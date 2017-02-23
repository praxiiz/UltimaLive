/* @file
 *
 * Copyright(c) 2016 UltimaLive
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CLIENT_STRUCTURES_H
#define _CLIENT_STRUCTURES_H

#include <stdint.h>

#pragma region Forward Declarations
struct CDrawItem;
struct CDrawItemVtable;
struct CDrawItemVtableMembers;
struct CDrawItemMembers;

struct CBigMobComponent;
struct CBigMobComponentMembers;

struct CTerrain;
struct CTextTerrain;

struct CStaticObject;
struct CStaticObjectVtable;
struct CStaticObjectVtableMembers;
struct CStaticObjectMembers;

struct CItem;
struct CItemVtable;
struct CItemVtableMembers;
struct CItemMembers;

struct CContainer;
struct CContainerVtable;
struct CContainerMembers;

struct CCorpseContainer;
struct CCorpseContainerMembers;

struct CMobile;
struct CMobileVtable;
struct CMobileVtableMembers;
struct CMobileMembers;

struct CPlayerMobile;
struct CPlayerMobileMembers;

#pragma endregion

#pragma region CDrawItem

/**
 * @class CDrawItemMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CDrawItemMembers
{
  uint32_t bitPattern;  //!< Unknown
  uint32_t rect_x1;     //!< Unknown
  uint32_t rect_y1;     //!< Unknown
  uint32_t rect_x2;     //!< Unknown
  uint32_t rect_y2;     //!< Unknown
  CDrawItem* pNextDraw; //!< Unknown
  CDrawItem* pPrevDraw; //!< Unknown
  uint32_t inDrawList;  //!< Unknown
  uint16_t x;		    //!< Unknown
  uint16_t y;		    //!< Unknown
  uint16_t z;		    //!< Unknown
};

/**
 * @class CDrawItemVtableMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CDrawItemVtableMembers
{
  CDrawItem *(__thiscall *Destroy)(void*, DWORD);           //!< Unknown
  int32_t (__thiscall *Release)(void*);						//!< Unknown
  int32_t DrawClip;											//!< Unknown
  int32_t DrawSomething;									//!< Unknown
  int32_t (__thiscall *AddToDrawList)(void*);				//!< Unknown
  void (__thiscall *RemoveFromDrawList)(void*);
  int32_t (__thiscall *IsStatic)(void*);					//!< Unknown
  int32_t (__thiscall *IsTerrain)(void*);					//!< Unknown
  int32_t (__thiscall *IsTextTerrain)(void*);				//!< Unknown
  int32_t (__thiscall *IsMobile)(void*);					//!< Unknown
  int32_t (__thiscall *IsPlayerMobile)(void*);				//!< Unknown
  int32_t (__thiscall *IsDynamic)(void*);					//!< Unknown
  void (__thiscall *IsContainer)(void*, int);
  int32_t (__thiscall *IsGumpArt)(void*);					//!< Unknown
  int32_t (__thiscall *IsBigMobComponent)(void*);			//!< Unknown
  int32_t (__thiscall *CheckShouldDrawTransparent)(void*);	//!< Unknown
  int32_t (__thiscall *GetFlags)(void*);					//!< Unknown
  int32_t (__thiscall *GetTopZ)(void*);						//!< Unknown
  int32_t GetOtherZ;										//!< Unknown
  int32_t getOtherZZ;										//!< Unknown
  void (__thiscall *Unequip)(void*);
  int32_t (__stdcall *field_54)(DWORD, DWORD, DWORD);		//!< Unknown
  int32_t (__thiscall *field_58)(DWORD, DWORD);				//!< Unknown
  int32_t (__thiscall *field_5C)(DWORD);					//!< Unknown
};

/**
 * @class CDrawItemVtable
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CDrawItemVtable
{
  /* 0x00 */ CDrawItemVtableMembers drawItemVtableMembers; //!< Unknown
};

/**
 * @class CDrawItem
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CDrawItem
{
  CDrawItemVtable* pVtable;			//!< Unknown
  CDrawItemMembers drawItemMembers; //!< Unknown
};
#pragma endregion


#pragma region CBigMobComponent
/**
 * @class CBigMobComponentMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CBigMobComponentMembers
{
  int field_0;					  //!< Unknown
  int field_4;					  //!< Unknown
  int field_8;					  //!< Unknown
  char gap_C[12];				  //!< Unknown
  int field_18;					  //!< Unknown
  int field_1C;					  //!< Unknown
  CContainer* pMyOwner;			  //!< Unknown
  CBigMobComponent *pNextMobComp; //!< Unknown
  CBigMobComponent *pPrevMobComp; //!< Unknown
};

/**
 * @class CBigMobComponent
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CBigMobComponent
{
  CDrawItemVtable* pVtable;						  //!< Unknown
  CDrawItemMembers drawItemMembers;				  //!< Unknown
  CBigMobComponentMembers bigMobComponentMembers; //!< Unknown
};
#pragma endregion

/**
 * @class CTerrain
 *
 * @brief Unknown
 */
#pragma region Terrain
struct CTerrain
{
  CDrawItemVtable* pVtable;         //!< Unknown
  CDrawItemMembers drawItemMembers;	//!< Unknown
  int32_t xPlusMapWidthMinusY;		//!< Unknown
  int32_t xPlusY;					//!< Unknown
  int32_t zOrder;					//!< Unknown
  char gap_36[4];					//!< Unknown
  int16_t graphicID;				//!< Unknown
  int16_t field_3C;					//!< Unknown
};

/**
 * @class CTextTerrain
 *
 * @brief Unknown
 */
struct CTextTerrain
{
  CDrawItemVtable* pVtable;         //!< Unknown
  CDrawItemMembers drawItemMembers;	//!< Unknown
  int32_t xPlusMapWidthMinusY;		//!< Unknown
  int32_t xPlusY;					//!< Unknown
  int32_t zOrder;					//!< Unknown
  char gap_36[4];					//!< Unknown
  int field_3A;						//!< Unknown
  char field_3E;					//!< Unknown
  char gap_3F[15];					//!< Unknown
  int firstTriangleIdx0;			//!< Unknown
  int firstTriangleIdx1;			//!< Unknown
  int firstTriangleIdx2;			//!< Unknown
  int secondTriangleIdx0;			//!< Unknown
  int secondTriangleIdx1;			//!< Unknown
  int secondTriangleIdx2;			//!< Unknown
  int field_66;						//!< Unknown
  int field_6A;						//!< Unknown
  //TODO: Define TileCorners
  //TileCorners Corners;
  //char field_72a;
  //char field_73;
  //char field_74;
  //char field_72;
};


#pragma endregion

#pragma region StaticObject
/**
 * @class CStaticObjectMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CStaticObjectMembers
{
  uint32_t xPlusMapWidthMinusY;			  //!< Unknown
  uint32_t xPlusY;						  //!< Unknown
  uint32_t zOrder;						  //!< Unknown
  uint32_t field36;						  //!< Unknown
  uint16_t itemID;						  //!< Unknown
  uint16_t originalItemID;				  //!< Unknown
  uint8_t  stackID;						  //!< Unknown
  uint8_t amount;						  //!< Unknown
  //uint8_t field16  */					  //!< Unknown
  uint16_t hue;							  //!< Unknown
  uint16_t unknown10;					  //!< Unknown
  CStaticObject* pNextStaticObject;		  //!< Unknown
  CStaticObject* pPrevStaticObject;		  //!< Unknown
  CStaticObject* pNextLightSource;		  //!< Unknown
  CStaticObject* pPrevLightSource;		  //!< Unknown
  CStaticObject* pNextAnimationStatic;	  //!< Unknown
  CStaticObject* pPrevAnimationStatic;	  //!< Unknown
  CStaticObject* pNextCanopyStaticObject; //!< Unknown
  CStaticObject* pPrevCanopyStaticObject; //!< Unknown
  CStaticObject* pNextMultiComponent;	  //!< Unknown
  CStaticObject* pPrevMultiComponent;     //!< Unknown
};

/**
 * @class CStaticObjectVtableMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CStaticObjectVtableMembers
{
  int(__thiscall *Unknown8)(void*);		   //!< Unknown
  int(__thiscall *Unknown9)(void*);		   //!< Unknown
  int(__thiscall *Unknown10)(void*);	   //!< Unknown
  int(__thiscall *IsEquipped)(void* This); //!< Unknown
};

/**
 * @class CStaticObjectVtable
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CStaticObjectVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;			//!< Unknown
  CStaticObjectVtableMembers staticObjectVtableMembers; //!< Unknown
};

/**
 * @class CStaticObject
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CStaticObject
{
  CStaticObjectVtable* pVtable;				//!< Unknown
  CDrawItemMembers drawItemMembers;			//!< Unknown
  CStaticObjectMembers staticObjectMembers; //!< Unknown
};
#pragma endregion

#pragma region CItem
/**
 * @class CItemMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CItemMembers
{
  int32_t serial;                           //!< Unknown
  CItem* pParent;							//!< Unknown
  CItem* pParentContainer;					//!< Unknown
  CItem* pNext_CItem;						//!< Unknown
  CItem* pPrev_CItem;						//!< Unknown
  int32_t field_10;							//!< Unknown
  int32_t field_14;							//!< Unknown
  CStaticObject* pField_18_multiComponent;	//!< Unknown
  char flags;								//!< Unknown
  char equipmentPosition;					//!< Unknown
  int32_t lightLevel;						//!< Unknown
  char* pName;								//!< Unknown
  int32_t price;							//!< Unknown
};

/**
 * @class CItemVtableMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CItemVtableMembers
{
  int field_0; //!< Unknown
};

/**
 * @class CItemVtable
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CItemVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;         //!< Unknown
  CStaticObjectVtableMembers staticObjectVtableMembers;	//!< Unknown
  CItemVtableMembers itemVtableMembers;					//!< Unknown
};

/**
 * @class CItem
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CItem
{
  CItemVtable* pVtable;                     //!< Unknown
  CDrawItemMembers drawItemMembers;			//!< Unknown
  CStaticObjectMembers staticObjectMembers;	//!< Unknown
  CItemMembers itemMembers;					//!< Unknown
};
#pragma endregion

#pragma region CContainer
/**
 * @class CContainerVtable
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CContainerVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;         //!< Unknown
  CStaticObjectVtableMembers staticObjectVtableMembers;	//!< Unknown
  CItemVtableMembers itemVtableMembers;					//!< Unknown
};

/**
 * @class CContainerMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CContainerMembers
{
  CItem* pShopContents;                 //!< Unknown
  CContainer* pSecureTradeContents;		//!< Unknown
  CBigMobComponent* pBigMobComponents;	//!< Unknown
  int field_unknown;					//!< Unknown
  int field_C;							//!< Unknown
};

/**
 * @class CContainerObj
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CContainerObj
{
  CContainerVtable* pVtable;                //!< Unknown
  CDrawItemMembers drawItemMembers;			//!< Unknown
  CStaticObjectMembers staticObjectMembers;	//!< Unknown
  CItemMembers itemMembers;					//!< Unknown
  CContainerMembers containerMembers;		//!< Unknown
};
#pragma endregion

#pragma region CCorpseContainer
/**
 * @class CCorpseContainerMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CCorpseContainerMembers
{
  int32_t field_B1;						    //!< Unknown
  int32_t field_B5;						    //!< Unknown
  int32_t field_B9;						    //!< Unknown
  int32_t field_BD;						    //!< Unknown
  int32_t aCorpseEquipmentLayerSerials[27]; //!< Unknown
};

/**
 * @class CCorpseContainer
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CCorpseContainer
{
  CContainerVtable* pVtable;                      //!< Unknown
  CDrawItemMembers drawItemMembers;				  //!< Unknown
  CStaticObjectMembers staticObjectMembers;		  //!< Unknown
  CItemMembers itemMembers;						  //!< Unknown
  CContainerMembers containerMembers;			  //!< Unknown
  CCorpseContainerMembers corpseContainerMembers; //!< Unknown
};
#pragma endregion

#pragma region CMobile

/**
 * @class CMobileMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CMobileMembers
{
  CMobile *pNextMobile;            //!< Unknown
  CMobile *pPrevMobile;			   //!<	Unknown
  void* aEquippedItems[30];		   //!<	Unknown
  int32_t field_80;				   //!<	Unknown
  int32_t field_84;				   //!<	Unknown
  int32_t field_88;				   //!<	Unknown
  int32_t field_8C;				   //!<	Unknown
  int32_t field_90;				   //!<	Unknown
  int32_t field_94;				   //!<	Unknown
  char vectorOfFour;			   //!<	Unknown
  char aGap_99[39];				   //!<	Unknown
  char field_C0;				   //!<	Unknown
  char aGap_C1[5];				   //!<	Unknown
  int32_t field_C6;				   //!<	Unknown
  char aGap_CA[8];				   //!<	Unknown
  int32_t field_D2;				   //!<	Unknown
  int32_t field_D6;				   //!<	Unknown
  int32_t field_DA;				   //!<	Unknown
  char aGap_DE[4];				   //!<	Unknown
  char field_E2;				   //!<	Unknown
  int32_t field_E3;				   //!<	Unknown
  int16_t field_E7;				   //!<	Unknown
  int16_t field_E9;				   //!<	Unknown
  int16_t field_EB;				   //!<	Unknown
  char aGap_ED[10];				   //!<	Unknown
  int16_t field_F7;				   //!<	Unknown
  int32_t field_F9;				   //!<	Unknown
  int32_t field_FD;				   //!<	Unknown
  int32_t field_101;			   //!<	Unknown
  int32_t field_105;			   //!<	Unknown
  int32_t field_109;			   //!<	Unknown
  int32_t field_10D;			   //!<	Unknown
  int32_t field_111;			   //!<	Unknown
  int32_t notorietyHighlightColor; //!<	Unknown
  int32_t field_119;			   //!<	Unknown
};

/**
 * @class CMobileVtableMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CMobileVtableMembers
{
  int field_1;                                      //!< Unknown
  int field_2;										//!< Unknown
  int field_3;										//!< Unknown
  void (__thiscall *field_4)(DWORD);				//!< Unknown
  int32_t (__thiscall *field_5)(DWORD);				//!< Unknown
  int field_6;										//!< Unknown
  /**
  * @brief Updates Notoriety
  */
  void (__thiscall *UpdateNotoriety)(DWORD, DWORD);	//!< Unknown
  int IsNotInnocent;								//!< Unknown
  int IsNotBadGuy;									//!< Unknown
};

/**
 * @class CMobileVtable
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CMobileVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;         //!< Unknown
  CStaticObjectVtableMembers staticObjectVtableMembers;	//!< Unknown
  CItemVtableMembers itemVtableMembers;					//!< Unknown
  CMobileVtableMembers mobileVtableMembers; 			//!< Unknown
};

/**
 * @class CMobile
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CMobile
{
  CMobileVtable* pVtable;                   //!< Unknown
  CDrawItemMembers drawItemMembers;		    //!< Unknown
  CStaticObjectMembers staticObjectMembers; //!< Unknown
  CItemMembers itemMembers;				    //!< Unknown
  CContainerMembers containerMembers;	    //!< Unknown
  CMobileMembers mobileMembers;			    //!< Unknown
};
#pragma endregion

#pragma region CPlayerMobile

/**
 * @class CPlayerMobileMembers
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CPlayerMobileMembers
{
  int32_t hitpoints;         //!< Unknown
  int32_t maxHitpoints;		 //!< Unknown
  int32_t mana;				 //!< Unknown
  int32_t maxMana;			 //!< Unknown
  int32_t stamina;			 //!< Unknown
  int32_t maxStamina;		 //!< Unknown
  int32_t strength;			 //!< Unknown
  int32_t dexterity;		 //!< Unknown
  int32_t intelligence;		 //!< Unknown
  CMobile *pAttackingMobile; //!< Unknown
  int32_t followingMobile;	 //!< Unknown
  int32_t field_2C;			 //!< Unknown
  int32_t field_30;			 //!< Unknown
  int32_t field_34;			 //!< Unknown
  int32_t field_38;			 //!< Unknown
  int32_t field_3C;			 //!< Unknown
  int32_t field_40;			 //!< Unknown
  int32_t walkKeys[5];		 //!< Unknown
  char field_58;			 //!< Unknown
};

/**
 * @class CPlayerMobile
 *
 * @brief Unknown
 */
#pragma pack(1)
struct CPlayerMobile
{
  CMobileVtable* pVtable;                   //!< Unknown
  CDrawItemMembers drawItemMembers;			//!< Unknown
  CStaticObjectMembers staticObjectMembers;	//!< Unknown
  CItemMembers itemMembers;					//!< Unknown
  CContainerMembers containerMembers;		//!< Unknown
  CMobileMembers mobileMembers;				//!< Unknown
  CPlayerMobileMembers playerMobileMembers;	//!< Unknown
};
#pragma endregion

/**
 * @class MapTileDefinition
 *
 * @brief Unknown
 */
#pragma pack(1)
struct MapTileDefinition
{
    uint32_t mapWidthInTiles;	   //!< Unknown
    uint32_t mapHeightInTiles;	   //!< Unknown
    uint32_t mapWrapWidthInTiles;  //!<	Unknown
    uint32_t mapWrapHeightInTiles; //!<	Unknown
};

/**
 * @class MapMinDisplay
 *
 * @brief Unknown
 */
#pragma pack(1)
struct MapMinDisplay
{
    uint32_t minClientDisplayX; //!< Unknown
    uint32_t minClientDisplayY; //!< Unknown
};

#endif