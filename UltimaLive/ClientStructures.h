/* Copyright(c) 2016 UltimaLive
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

#pragma pack(1)
struct CDrawItemMembers
{
  uint32_t bitPattern;
  uint32_t rect_x1; //field 8
  uint32_t rect_y1; //field c
  uint32_t rect_x2; //field 10
  uint32_t rect_y2; //field 14
  CDrawItem* pNextDraw;
  CDrawItem* pPrevDraw;
  uint32_t inDrawList;
  uint16_t x;
  uint16_t y;
  uint16_t z;
};

#pragma pack(1)
struct CDrawItemVtableMembers
{
  CDrawItem *(__thiscall *Destroy)(void*, DWORD);
  int32_t (__thiscall *Release)(void*);
  int32_t DrawClip;
  int32_t DrawSomething;
  int32_t (__thiscall *AddToDrawList)(void*);
  void (__thiscall *RemoveFromDrawList)(void*);
  int32_t (__thiscall *IsStatic)(void*);
  int32_t (__thiscall *IsTerrain)(void*);
  int32_t (__thiscall *IsTextTerrain)(void*);
  int32_t (__thiscall *IsMobile)(void*);
  int32_t (__thiscall *IsPlayerMobile)(void*);
  int32_t (__thiscall *IsDynamic)(void*);
  void (__thiscall *IsContainer)(void*, int);
  int32_t (__thiscall *IsGumpArt)(void*);
  int32_t (__thiscall *IsBigMobComponent)(void*);
  int32_t (__thiscall *CheckShouldDrawTransparent)(void*);
  int32_t (__thiscall *GetFlags)(void*);
  int32_t (__thiscall *GetTopZ)(void*);
  int32_t GetOtherZ;
  int32_t getOtherZZ;
  void (__thiscall *Unequip)(void*);
  int32_t (__stdcall *field_54)(DWORD, DWORD, DWORD);
  int32_t (__thiscall *field_58)(DWORD, DWORD);
  int32_t (__thiscall *field_5C)(DWORD);
};


#pragma pack(1)
struct CDrawItemVtable
{
  /* 0x00 */ CDrawItemVtableMembers drawItemVtableMembers;
};

#pragma pack(1)
struct CDrawItem
{
  CDrawItemVtable* pVtable;
  CDrawItemMembers drawItemMembers;
};
#pragma endregion

#pragma region CBigMobComponent
#pragma pack(1)
struct CBigMobComponentMembers
{
  int field_0;
  int field_4;
  int field_8;
  char gap_C[12];
  int field_18;
  int field_1C;
  CContainer* pMyOwner;
  CBigMobComponent *pNextMobComp;
  CBigMobComponent *pPrevMobComp;
};

#pragma pack(1)
struct CBigMobComponent
{
  CDrawItemVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CBigMobComponentMembers bigMobComponentMembers;
};
#pragma endregion

#pragma region Terrain
struct CTerrain
{
  CDrawItemVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  int32_t xPlusMapWidthMinusY;
  int32_t xPlusY;
  int32_t zOrder;
  char gap_36[4];
  int16_t graphicID;
  int16_t field_3C;
};

struct CTextTerrain
{
  CDrawItemVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  int32_t xPlusMapWidthMinusY;
  int32_t xPlusY;
  int32_t zOrder;
  char gap_36[4];
  int field_3A;
  char field_3E;
  char gap_3F[15];
  int firstTriangleIdx0;
  int firstTriangleIdx1;
  int firstTriangleIdx2;
  int secondTriangleIdx0;
  int secondTriangleIdx1;
  int secondTriangleIdx2;
  int field_66;
  int field_6A;
  //TODO: Define TileCorners
  //TileCorners Corners;
  //char field_72a;
  //char field_73;
  //char field_74;
  //char field_72;
};


#pragma endregion

#pragma region StaticObject
#pragma pack(1)
struct CStaticObjectMembers
{
  uint32_t xPlusMapWidthMinusY;
  uint32_t xPlusY;
  uint32_t zOrder;
  uint32_t field36;
  uint16_t itemID;
  uint16_t originalItemID;
  uint8_t  stackID;
  uint8_t amount;
  //uint8_t field16  */
  uint16_t hue;
  uint16_t unknown10;
  CStaticObject* pNextStaticObject;
  CStaticObject* pPrevStaticObject;
  CStaticObject* pNextLightSource;
  CStaticObject* pPrevLightSource;
  CStaticObject* pNextAnimationStatic;
  CStaticObject* pPrevAnimationStatic;
  CStaticObject* pNextCanopyStaticObject;
  CStaticObject* pPrevCanopyStaticObject;
  CStaticObject* pNextMultiComponent;
  CStaticObject* pPrevMultiComponent;
};

#pragma pack(1)
struct CStaticObjectVtableMembers
{
  int(__thiscall *Unknown8)(void*);
  int(__thiscall *Unknown9)(void*);
  int(__thiscall *Unknown10)(void*);
  int(__thiscall *IsEquipped)(void* This);
};

#pragma pack(1)
struct CStaticObjectVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;
  CStaticObjectVtableMembers staticObjectVtableMembers;
};

#pragma pack(1)
struct CStaticObject
{
  CStaticObjectVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
};
#pragma endregion

#pragma region CItem
#pragma pack(1)
struct CItemMembers
{
  int32_t serial;
  CItem* pParent;
  CItem* pParentContainer;
  CItem* pNext_CItem;
  CItem* pPrev_CItem;
  int32_t field_10;
  int32_t field_14;
  CStaticObject* pField_18_multiComponent;
  char flags;
  char equipmentPosition;
  int32_t lightLevel;
  char* pName;
  int32_t price;
};

#pragma pack(1)
struct CItemVtableMembers
{
  int field_0;
};

#pragma pack(1)
struct CItemVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;
  CStaticObjectVtableMembers staticObjectVtableMembers;
  CItemVtableMembers itemVtableMembers;
};

#pragma pack(1)
struct CItem
{
  CItemVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
  CItemMembers itemMembers;
};
#pragma endregion

#pragma region CContainer
#pragma pack(1)
struct CContainerVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;
  CStaticObjectVtableMembers staticObjectVtableMembers;
  CItemVtableMembers itemVtableMembers;
};

#pragma pack(1)
struct CContainerMembers
{
  CItem* pShopContents;
  CContainer* pSecureTradeContents;
  CBigMobComponent* pBigMobComponents;
  int field_unknown;
  int field_C;
};

#pragma pack(1)
struct CContainerObj
{
  CContainerVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
  CItemMembers itemMembers;
  CContainerMembers containerMembers;
};
#pragma endregion

#pragma region CCorpseContainer
#pragma pack(1)
struct CCorpseContainerMembers
{
  int32_t field_B1;
  int32_t field_B5;
  int32_t field_B9;
  int32_t field_BD;
  int32_t aCorpseEquipmentLayerSerials[27];
};

#pragma pack(1)
struct CCorpseContainer
{
  CContainerVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
  CItemMembers itemMembers;
  CContainerMembers containerMembers;
  CCorpseContainerMembers corpseContainerMembers;
};
#pragma endregion

#pragma region CMobile

#pragma pack(1)
struct CMobileMembers
{
  CMobile *pNextMobile;
  CMobile *pPrevMobile;
  void* aEquippedItems[30];
  int32_t field_80;
  int32_t field_84;
  int32_t field_88;
  int32_t field_8C;
  int32_t field_90;
  int32_t field_94;
  char vectorOfFour;
  char aGap_99[39];
  char field_C0;
  char aGap_C1[5];
  int32_t field_C6;
  char aGap_CA[8];
  int32_t field_D2;
  int32_t field_D6;
  int32_t field_DA;
  char aGap_DE[4];
  char field_E2;
  int32_t field_E3;
  int16_t field_E7;
  int16_t field_E9;
  int16_t field_EB;
  char aGap_ED[10];
  int16_t field_F7;
  int32_t field_F9;
  int32_t field_FD;
  int32_t field_101;
  int32_t field_105;
  int32_t field_109;
  int32_t field_10D;
  int32_t field_111;
  int32_t notorietyHighlightColor;
  int32_t field_119;
};

#pragma pack(1)
struct CMobileVtableMembers
{
  int field_1;
  int field_2;
  int field_3;
  void (__thiscall *field_4)(DWORD);
  int32_t (__thiscall *field_5)(DWORD);
  int field_6;
  void (__thiscall *UpdateNotoriety)(DWORD, DWORD);
  int IsNotInnocent;
  int IsNotBadGuy;
};

#pragma pack(1)
struct CMobileVtable
{
  CDrawItemVtableMembers drawItemVtableMembers;
  CStaticObjectVtableMembers staticObjectVtableMembers;
  CItemVtableMembers itemVtableMembers;
  CMobileVtableMembers mobileVtableMembers; 
};

#pragma pack(1)
struct CMobile
{
  CMobileVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
  CItemMembers itemMembers;
  CContainerMembers containerMembers;
  CMobileMembers mobileMembers;
};
#pragma endregion

#pragma region CPlayerMobile

#pragma pack(1)
struct CPlayerMobileMembers
{
  int32_t hitpoints;
  int32_t maxHitpoints;
  int32_t mana;
  int32_t maxMana;
  int32_t stamina;
  int32_t maxStamina;
  int32_t strength;
  int32_t dexterity;
  int32_t intelligence;
  CMobile *pAttackingMobile;
  int32_t followingMobile;
  int32_t field_2C;
  int32_t field_30;
  int32_t field_34;
  int32_t field_38;
  int32_t field_3C;
  int32_t field_40;
  int32_t walkKeys[5];
  char field_58;
};

#pragma pack(1)
struct CPlayerMobile
{
  CMobileVtable* pVtable;
  CDrawItemMembers drawItemMembers;
  CStaticObjectMembers staticObjectMembers;
  CItemMembers itemMembers;
  CContainerMembers containerMembers;
  CMobileMembers mobileMembers;
  CPlayerMobileMembers playerMobileMembers;
};
#pragma endregion

#pragma pack(1)
struct MapTileDefinition
{
    uint32_t mapWidthInTiles;
    uint32_t mapHeightInTiles;
    uint32_t mapWrapWidthInTiles;
    uint32_t mapWrapHeightInTiles;
};

#pragma pack(1)
struct MapMinDisplay
{
    uint32_t minClientDisplayX;
    uint32_t minClientDisplayY;
};

#endif