
// Définitions des arguments
#include "DefinitionsArguments.h"
// LoadCel()
#include "celutils.h"
// Point2D, Quad2D
#include "Geometry.h"
// printf()
#include "stdio.h"

int32 main(){
	
	// Cel
	CCB *cel_1 = NULL;
	CCB *cel_2 = NULL;
	// GroupCel
	GroupCel *groupCel = NULL;
	
	// Transformations
	int32 rotationAngle = 0;
	Point2D rotationPivot = {0};
	Quad2D stretch = {0};
	
	// Charge le CEL
	// printf("LoadCel()\n");
	cel_1 = LoadCel("image_1.cel", MEMTYPE_DRAM);
	// Si c'est un échec
	if(cel_1 == NULL){
		// Retourne une erreur
		printf("Error <- LoadCel()\n");
		return -1;
	}	

	// Charge le CEL
	// printf("LoadCel()\n");
	cel_2 = LoadCel("image_2.cel", MEMTYPE_DRAM);
	// Si c'est un échec
	if(cel_2 == NULL){
		// Retourne une erreur
		printf("Error <- LoadCel()\n");
		return -1;
	}	
	
	// Crée un GroupCel
	// printf("GroupCelInitialization()\n");
	groupCel = GroupCelInitialization(
		// Position
		10, 10, 
		// Dimension
		20, 30, 
		// Nombre de Cels
		2
	);
	// Si c'est un échec
	if(groupCel == NULL){
		// Retourne une erreur
		printf("Error <- GroupCelInitialization()\n");
		return -1;
	}	
	
	// Configure le GroupCel
	printf("-> GroupCelElementsConfiguration()\n");
	if (GroupCelElementsConfiguration(
		// GroupCel
		groupCel,
		// Début de la liste
		LIST_START,
		// celIndex, CCB, positionX, positionY
		0, cel_1, 0, 0,
		1, cel_2, 0, 0,
		// Fin de la liste
		LIST_END
	) == 0) {
		printf("Error <- GroupCelElementsConfiguration()\n");
		return -1;
	}	
	
	// Prépare les transformations
	rotationAngle = 128;
	rotationPivot.x = 50;
	rotationPivot.y = 50;
	stretch.bottomRight.x = 10;
	
	// Rotation
	GroupCelRotate(groupCel, rotationAngle, character->position.x, character->position.y);
	
	// Etirement
	GroupCelStretch(groupCel, 
		// Point 0
		groupCel->position.x + stretch.topLeft.x,
		groupCel->position.y + stretch.topLeft.y,
		// Point 1
		groupCel->position.x + groupCel->size.x + stretch.topRight.x,
		groupCel->position.y + stretch.topRight.y,
		// Point 2
		groupCel->position.x + groupCel->size.x + stretch.bottomRight.x,
		groupCel->position.y + groupCel->size.y + stretch.bottomRight.y,
		// Point 3
		groupCel->position.x + stretch.bottomLeft.x,
		groupCel->position.y + groupCel->size.y + stretch.bottomLeft.y
	);

	// Met à jour les élément dans le GroupCel
	GroupCelUpdate(groupCel);
	
	// Supprime le GroupCel
	SpriteCelCleanup(groupCel);
	
	// Supprime les Cels
	UnloadCel(cel_1);
	UnloadCel(cel_2);
	
    // Termine le programme
    return 0;
}