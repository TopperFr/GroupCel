#ifndef GROUPCEL_H
#define GROUPCEL_H

/******************************************************************************
**
**  GroupCel - Groupe de Cels, SpriteCels ou AnimSpriteCels (3DO Cel Engine)
**  
**  Auteur : Christophe Geoffroy (Topper) - Licence MIT
**  Dernière mise à jour : 05-08-2025
**
**  Ce module permet de regrouper des Cels, SpriteCels ou AnimSpriteCels dans
**  une seule structure. Il permet d'effectuer des déplacements, étirements et
**  rotations de tous les éléments du GroupCel comme si c'était un object unique.
**  
**  Chaque Cel, SpriteCel ou AnimSpriteCel garde son indépendance. Ainsi, il est
**  possible de modifier la position ou l'affichage d'un élément dans le GroupCel.
**  
**  D'autres éléments peuvent être ajoutées en fournissant le CCB et une position,
**  comme un TextCel par exemple.
** 
**  Notes importantes :
**
**    - Le GroupCel possède un cadre rectangulaire déterminé par sa position et
**      sa taille. Il est conseillé de donner à ce cadre une dimension qui englobe
**      l'ensemble des éléments. Ce n'est pas une obligation mais l'étirement prend
**      en compte les coins de ce cadre. Placer des éléments à l'extérieur du cadre
**      pourrait provoquer des résultats inattendus.
**
**    - Les éléments sont ajoutés directement à la structure sans copie. 
**      Si un Cel est utilisé à l'extérieur du groupe, faire un CloneCel()
**      avant l'ajout.
**
**    - Le pivot de rotation est relatif à l'écran. Il est donc possible de choisir 
**      un pivot à l'extérieur ou à l'intérieur du cadre du GroupCel.  
**
**    - Chaque modification du GroupCel (déplacement, étirement, rotation) doit être
**      appliqué par la fonction de mise à jour GroupCelUpdate(). Cette fonction  
**      contrôle les modifications demandées et réalise uniquement les calculs nécessaires.
**      Un seul appel est nécessaire pour toutes les modifications sur tous les cels.
**
**    - Tous les Cels dans le groupe sont liés automatiquement. Un seul appel à DrawCels()
**      permet de projeter tous les Cels du GroupCel. Pour ajouter le GroupCel à une liste 
**      de Cels, il faut pointer sur le CCB principal. Pour ajouter d'autres Cels à la suite, 
**      il faut continuer la liaison à partir du CCB du dernier Cel ajouté.
**
**    - Lorsqu'un GroupCel est initialisé, il doit être supprimé avec GroupCelCleanup().
**      Les éléments qui lui sont associés doivent être supprimés indépendamment.
**
**  Rôle des structures :
**
**    GroupCelList
**      - position : Position du l'élément dans le GroupCel
**      - cel : CCB de l'élément
**
**    GroupCel
**      - cel : CCB actif affiché à l'écran
**      - position : Position du GroupCel
**      - size : Taille du GroupCel
**      - stretching : Stockage des paramètres d'étirement
**      - rotationAngle : Stockage de l'angle de rotation
**      - rotationPivot : Stockage du point de pivot de rotation
**      - transformations : Flags pour déterminer les transformations à appliquer
**      - celsCount : nombre total de Cels dans le GroupCel
**      - cels : tableau dynamique de "GroupCelList"
**
**  Fonctions principales :
**
**    GroupCelInitialization()
**      -> Initialise le GroupCel, le cadre et prépare le tableau des éléments.
**
**    GroupCelElementConfiguration()
**      -> Définit un élément du GroupCel : position et CCB.
**
**    GroupCelElementsConfiguration()
**      -> Définit plusieurs éléments du GroupCel en une seule passe avec des arguments variadiques.
**
**    GroupCelMove()
**      -> Déplace le GroupCel en valeurs relatives par rapport à lui-même.
**
**    GroupCelSetPosition()
**      -> Déplace le GroupCel en valeurs absolues.
**
**    GroupCelElementMove()
**      -> Déplace un élément dans le GroupCel en valeurs relatives par rapport à lui-même.
**         L'appel de GroupCelUpdate() est nécessaire pour la prise en compte des modifications
**
**    GroupCelElementSetPosition()
**      -> Déplace un élément dans le GroupCel en valeurs absolues dans le cadre du GroupCel.
**         L'appel de GroupCelUpdate() est nécessaire pour la prise en compte des modifications
**
**    GroupCelStretch()
**      -> Etire le GroupCel en modifiant les quatre coins du cadre en valeurs absolues.
**         L'appel de GroupCelUpdate() est nécessaire pour la prise en compte des modifications
**
**    GroupCelRotate()
**      -> Fais tourner le GroupCel en indiquant un angle et un pivot en valeurs absolues.
**         L'appel de GroupCelUpdate() est nécessaire pour la prise en compte des modifications
**
**    GroupCelUpdate()
**      -> Mets à jour les élément dans le GroupCel
**
**    GroupCelElementUpdatePosition()
**      -> Fonction interne appliquant les modifications de position.
**
**    GroupCelUpdateTransformations()
**      -> Fonction interne appliquant les modifications d'étirement et de rotation.
**
**    GroupCelCleanup()
**      -> Libère la mémoire utilisée par la structure GroupCel.
**
******************************************************************************/

// CCB
#include "graphics.h"
// Point2D, Quad2D
#include "Geometry.h"
// int32
#include "types.h"

// Debug
#define DEBUG_GROUPCEL_INIT 0
#define DEBUG_GROUPCEL_SETUP 0
#define DEBUG_GROUPCEL_FUNCT 0
#define DEBUG_GROUPCEL_CLEAN 0

// Flag transformations
// bit-0 -> position
#define GROUPCEL_FLAG_MOVE (1 << 0)
// bit-1 -> étirement
#define GROUPCEL_FLAG_STRETCH (1 << 1)
// bit-2 -> rotation
#define GROUPCEL_FLAG_ROTATE (1 << 2)

typedef struct {
	// Position
    Point2D position;
	// CCB du cel
	CCB *cel;
} GroupCelList;

typedef struct {
	// CCB principal du groupe
	CCB *cel;
	// Position
    Point2D position;
	// Dimension
    Point2D size;
    // Quadrilatère d'étirement en pixels (local 0..x, 0..y)
    Quad2D stretching;      
    // Paramètres de rotation 
	// -> Angle en 256 unités (90° = 64, 180° = 128, 270° = 192)
    uint32 rotationAngle;
	// -> Pivot absolu en pixels
    Point2D rotationPivot;
	// Transformations à réaliser (move = bit-0, stretch = bit-1, rotate = bit-2)
    int32 transformations;	
	// Nombre total de cels
    uint32 celsCount;
	// Tableau de cels
    GroupCelList *cels;
} GroupCel;

// Référence au contexte global
extern GroupCel groupCel;

// Initialisation d'un GroupCel
GroupCel *GroupCelInitialization(int32 positionX, int32 positionY, uint32 sizeX, uint32 sizeY, uint32 celsCount);
// Configuration d'un cel d'un GroupCel
int32 GroupCelElementConfiguration(GroupCel *groupCel, uint32 celIndex, CCB *cel, uint32 positionX, uint32 positionY);
// Configuration des cels d'un GroupCel
int32 GroupCelElementsConfiguration(GroupCel *groupCel, int32 start, ...);

// Déplace le GroupCel
void GroupCelMove(GroupCel *groupCel, int32 moveX, int32 moveY);
// Modifie la position du GroupCel
void GroupCelSetPosition(GroupCel *groupCel, int32 positionX, int32 positionY);

// Déplace un élément dans le GroupCel
void GroupCelElementMove(GroupCel *groupCel, int32 celIndex, int32 moveX, int32 moveY);
// Modifie la position d'un cel du GroupCel
void GroupCelElementSetPosition(GroupCel *groupCel, int32 celIndex, int32 positionX, int32 positionY);

// Etire le groupe selon ses 4 coins en pixels
void GroupCelStretch(GroupCel *groupCel, int32 point1X, int32 point1Y, int32 point2X, int32 point2Y, int32 point3X, int32 point3Y, int32 point4X, int32 point4Y);
// Tourne le groupe selon un angle en 256 unités
void GroupCelRotate(GroupCel *groupCel, uint32 angle, int32 pivotX, int32 pivotY);

// Met à jour les élément dans le GroupCel
void GroupCelUpdate(GroupCel *groupCel);
// Applique la position dans le CCB du Cel
void GroupCelElementUpdatePosition(GroupCel *groupCel, int32 celIndex, Quad2D *quadMap);
// Applique les transformations Stretch et Rotate dans le CCB des Cels
void GroupCelUpdateTransformations(GroupCel *groupCel, int32 doStretch, int32 doRotate, Quad2D *quadMap);

// Supprime le GroupCel
int32 GroupCelCleanup(GroupCel *groupCel);

#endif // GROUPCEL_H