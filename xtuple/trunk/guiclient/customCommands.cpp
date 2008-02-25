/*
 * Common Public Attribution License Version 1.0. 
 * 
 * The contents of this file are subject to the Common Public Attribution 
 * License Version 1.0 (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License 
 * at http://www.xTuple.com/CPAL.  The License is based on the Mozilla 
 * Public License Version 1.1 but Sections 14 and 15 have been added to 
 * cover use of software over a computer network and provide for limited 
 * attribution for the Original Developer. In addition, Exhibit A has 
 * been modified to be consistent with Exhibit B.
 * 
 * Software distributed under the License is distributed on an "AS IS" 
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See 
 * the License for the specific language governing rights and limitations 
 * under the License. 
 * 
 * The Original Code is PostBooks Accounting, ERP, and CRM Suite. 
 * 
 * The Original Developer is not the Initial Developer and is __________. 
 * If left blank, the Original Developer is the Initial Developer. 
 * The Initial Developer of the Original Code is OpenMFG, LLC, 
 * d/b/a xTuple. All portions of the code written by xTuple are Copyright 
 * (c) 1999-2008 OpenMFG, LLC, d/b/a xTuple. All Rights Reserved. 
 * 
 * Contributor(s): ______________________.
 * 
 * Alternatively, the contents of this file may be used under the terms 
 * of the xTuple End-User License Agreeement (the xTuple License), in which 
 * case the provisions of the xTuple License are applicable instead of 
 * those above.  If you wish to allow use of your version of this file only 
 * under the terms of the xTuple License and not to allow others to use 
 * your version of this file under the CPAL, indicate your decision by 
 * deleting the provisions above and replace them with the notice and other 
 * provisions required by the xTuple License. If you do not delete the 
 * provisions above, a recipient may use your version of this file under 
 * either the CPAL or the xTuple License.
 * 
 * EXHIBIT B.  Attribution Information
 * 
 * Attribution Copyright Notice: 
 * Copyright (c) 1999-2008 by OpenMFG, LLC, d/b/a xTuple
 * 
 * Attribution Phrase: 
 * Powered by PostBooks, an open source solution from xTuple
 * 
 * Attribution URL: www.xtuple.org 
 * (to be included in the "Community" menu of the application if possible)
 * 
 * Graphic Image as provided in the Covered Code, if any. 
 * (online at www.xtuple.com/poweredby)
 * 
 * Display of Attribution Information is required in Larger Works which 
 * are defined in the CPAL as a work which combines Covered Code or 
 * portions thereof with code not governed by the terms of the CPAL.
 */

#include "customCommands.h"

#include <QVariant>
#include <QMessageBox>

#include <parameter.h>

#include "customCommand.h"
#include "guiclient.h"

customCommands::customCommands(QWidget* parent, const char* name, Qt::WFlags fl)
    : XMainWindow(parent, name, fl)
{
  setupUi(this);

  (void)statusBar();

  connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
  connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
  connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));

  _commands->addColumn(tr("Module"), _itemColumn, Qt::AlignCenter );
  _commands->addColumn(tr("Menu Label"), -1,     Qt::AlignLeft );

  sFillList();
}

customCommands::~customCommands()
{
  // no need to delete child widgets, Qt does it all for us
}

void customCommands::languageChange()
{
  retranslateUi(this);
}

void customCommands::sNew()
{
  ParameterList params;
  params.append("mode", "new");

  customCommand newdlg(this, "", TRUE);
  newdlg.set(params);
  if(newdlg.exec() == XDialog::Accepted)
    sFillList();
}

void customCommands::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("cmd_id", _commands->id());

  customCommand newdlg(this, "", TRUE);
  newdlg.set(params);
  if(newdlg.exec() == XDialog::Accepted)
    sFillList();
}

void customCommands::sDelete()
{
  q.prepare("BEGIN; "
            "DELETE FROM cmdarg WHERE (cmdarg_cmd_id=:cmd_id); "
            "DELETE FROM cmd WHERE (cmd_id=:cmd_id); "
            "SELECT updateCustomPrivs(); "
            "COMMIT; ");
  q.bindValue(":cmd_id", _commands->id());
  if(q.exec())
    sFillList();
  else
    systemError( this, tr("A System Error occurred at customCommands::%1")
                             .arg(__LINE__) );
}

void customCommands::sFillList()
{
  QString sql = ("SELECT cmd_id, cmd_module, cmd_title"
            "  FROM cmd"
            "  WHERE ((cmd_module='CRM') "
            "  OR (cmd_module ");
            
  //Remove this when old menu system goes away
  if (_preferences->boolean("UseOldMenu"))
    sql += " NOT ";
  
  sql +=    " IN ('Products','Inventory','Schedule','Purchase', "
            " 'Manufacture','CRM','Sales','Accounting','System'))) "
            " ORDER BY cmd_module, cmd_title;";
  q.prepare(sql);
  q.exec();
  _commands->populate(q);
}
