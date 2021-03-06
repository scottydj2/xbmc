/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "guilib/guiinfo/GUIInfoProviders.h"
#include "interfaces/info/InfoBool.h"
#include "interfaces/info/SkinVariable.h"
#include "messaging/IMessageTarget.h"
#include "threads/CriticalSection.h"
#include "utils/Observer.h"

class CFileItem;
class CVideoInfoTag;

class CGUIListItem;
typedef std::shared_ptr<CGUIListItem> CGUIListItemPtr;

namespace KODI
{
namespace GUILIB
{
namespace GUIINFO
{
  class CGUIInfo;
  class IGUIInfoProvider;
}
}
}
namespace INFO
{
  class InfoSingle;
}
namespace MUSIC_INFO
{
  class CMusicInfoTag;
}

/*!
 \ingroup strings
 \brief
 */
class CGUIInfoManager : public Observable, public KODI::MESSAGING::IMessageTarget
{
public:
  CGUIInfoManager(void);
  ~CGUIInfoManager(void) override;

  void Initialize();

  void Clear();
  void ResetCache();

  // KODI::MESSAGING::IMessageTarget implementation
  int GetMessageMask() override;
  void OnApplicationMessage(KODI::MESSAGING::ThreadMessage* pMsg) override;

  /*! \brief Register a boolean condition/expression
   This routine allows controls or other clients of the info manager to register
   to receive updates of particular expressions, in a particular context (currently windows).

   In the future, it will allow clients to receive pushed callbacks when the expression changes.

   \param expression the boolean condition or expression
   \param context the context window
   \return an identifier used to reference this expression
   */
  INFO::InfoPtr Register(const std::string &expression, int context = 0);

  /// \brief iterates through boolean conditions and compares their stored values to current values. Returns true if any condition changed value.
  bool ConditionsChangedValues(const std::map<INFO::InfoPtr, bool>& map);

  /*! \brief Evaluate a boolean expression
   \param expression the expression to evaluate
   \param context the context in which to evaluate the expression (currently windows)
   \return the value of the evaluated expression.
   \sa Register
   */
  bool EvaluateBool(const std::string &expression, int context = 0, const CGUIListItemPtr &item = nullptr);

  int TranslateString(const std::string &strCondition);
  int TranslateSingleString(const std::string &strCondition, bool &listItemDependent);

  std::string GetLabel(int info, int contextWindow = 0, std::string *fallback = nullptr) const;
  std::string GetImage(int info, int contextWindow, std::string *fallback = nullptr);
  bool GetInt(int &value, int info, int contextWindow = 0, const CGUIListItem *item = nullptr) const;
  bool GetBool(int condition, int contextWindow = 0, const CGUIListItem *item = nullptr);

  std::string GetItemLabel(const CFileItem *item, int contextWindow, int info, std::string *fallback = nullptr) const;
  std::string GetItemImage(const CFileItem *item, int contextWindow, int info, std::string *fallback = nullptr) const;
  /*! \brief Get integer value of info.
   \param value int reference to pass value of given info
   \param info id of info
   \param context the context in which to evaluate the expression (currently windows)
   \param item optional listitem if want to get listitem related int
   \return true if given info was handled
   \sa GetItemInt, GetMultiInfoInt
   */
  bool GetItemInt(int &value, const CGUIListItem *item, int contextWindow, int info) const;
  bool GetItemBool(const CGUIListItem *item, int contextWindow, int condition) const;

  /*! \brief Set currently playing file item
   */
  void SetCurrentItem(const CFileItem &item);
  void ResetCurrentItem();
  void UpdateCurrentItem(const CFileItem &item);

  // Current song stuff
  void SetCurrentAlbumThumb(const std::string &thumbFileName);
  const MUSIC_INFO::CMusicInfoTag *GetCurrentSongTag() const;

  // Current video stuff
  const CVideoInfoTag* GetCurrentMovieTag() const;

  void UpdateAVInfo();

  int RegisterSkinVariableString(const INFO::CSkinVariableString* info);
  int TranslateSkinVariableString(const std::string& name, int context);

  /*! \brief register a guiinfo provider
   \param the guiinfo provider to register
   */
  void RegisterInfoProvider(KODI::GUILIB::GUIINFO::IGUIInfoProvider *provider);

  /*! \brief unregister a guiinfo provider
   \param the guiinfo provider to unregister
   */
  void UnregisterInfoProvider(KODI::GUILIB::GUIINFO::IGUIInfoProvider *provider);

  /*! \brief get access to the registered guiinfo providers
   \return the guiinfo providers
   */
  KODI::GUILIB::GUIINFO::CGUIInfoProviders& GetInfoProviders() { return m_infoProviders; }

private:
  /*! \brief class for holding information on properties
   */
  class Property
  {
  public:
    Property(const std::string &property, const std::string &parameters);

    const std::string &param(unsigned int n = 0) const;
    unsigned int num_params() const;

    std::string name;
  private:
    std::vector<std::string> params;
  };

  /*! \brief Split an info string into it's constituent parts and parameters
   Format is:

     info1(params1).info2(params2).info3(params3) ...

   where the parameters are an optional comma separated parameter list.

   \param infoString the original string
   \param info the resulting pairs of info and parameters.
   */
  void SplitInfoString(const std::string &infoString, std::vector<Property> &info);

  int TranslateSingleString(const std::string &strCondition);
  int TranslateListItem(const Property& cat, const Property& prop, int id, bool container);
  int TranslateMusicPlayerString(const std::string &info) const;
  static TIME_FORMAT TranslateTimeFormat(const std::string &format);

  std::string GetMultiInfoLabel(const KODI::GUILIB::GUIINFO::CGUIInfo &info, int contextWindow, std::string *fallback = nullptr) const;
  bool GetMultiInfoInt(int &value, const KODI::GUILIB::GUIINFO::CGUIInfo &info, int contextWindow, const CGUIListItem *item) const;
  bool GetMultiInfoBool(const KODI::GUILIB::GUIINFO::CGUIInfo &info, int contextWindow, const CGUIListItem *item);

  std::string GetMultiInfoItemLabel(const CFileItem *item, int contextWindow, const KODI::GUILIB::GUIINFO::CGUIInfo &info, std::string *fallback = nullptr) const;
  std::string GetMultiInfoItemImage(const CFileItem *item, int contextWindow, const KODI::GUILIB::GUIINFO::CGUIInfo &info, std::string *fallback = nullptr) const;

  std::string GetSkinVariableString(int info, bool preferImage = false, const CGUIListItem *item = nullptr) const;

  int AddMultiInfo(const KODI::GUILIB::GUIINFO::CGUIInfo &info);

  void SetCurrentSongTag(const MUSIC_INFO::CMusicInfoTag &tag);
  void SetCurrentVideoTag(const CVideoInfoTag &tag);

  // Vector of multiple information mapped to a single integer lookup
  std::vector<KODI::GUILIB::GUIINFO::CGUIInfo> m_multiInfo;

  // Current playing stuff
  CFileItem* m_currentFile;

  typedef std::set<INFO::InfoPtr, bool(*)(const INFO::InfoPtr&, const INFO::InfoPtr&)> INFOBOOLTYPE;
  INFOBOOLTYPE m_bools;
  unsigned int m_refreshCounter;
  std::vector<INFO::CSkinVariableString> m_skinVariableStrings;

  CCriticalSection m_critInfo;

  KODI::GUILIB::GUIINFO::CGUIInfoProviders m_infoProviders;
};
