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

#include "locations.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qworkspace.h>
#include <openreports.h>
#include <parameter.h>
#include <qstatusbar.h>
#include "location.h"

/*
 *  Constructs a locations as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 */
locations::locations(QWidget* parent, const char* name, Qt::WFlags fl)
    : XMainWindow(parent, name, fl)
{
    setupUi(this);

    (void)statusBar();

    // signals and slots connections
    connect(_print, SIGNAL(clicked()), this, SLOT(sPrint()));
    connect(_new, SIGNAL(clicked()), this, SLOT(sNew()));
    connect(_edit, SIGNAL(clicked()), this, SLOT(sEdit()));
    connect(_delete, SIGNAL(clicked()), this, SLOT(sDelete()));
    connect(_close, SIGNAL(clicked()), this, SLOT(close()));
    connect(_location, SIGNAL(valid(bool)), _view, SLOT(setEnabled(bool)));
    connect(_view, SIGNAL(clicked()), this, SLOT(sView()));
    connect(_warehouse, SIGNAL(updated()), this, SLOT(sFillList()));
    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
locations::~locations()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void locations::languageChange()
{
    retranslateUi(this);
}


void locations::init()
{
  statusBar()->hide();
  
  _location->addColumn(tr("Whs."),        _whsColumn,  Qt::AlignCenter );
  _location->addColumn(tr("Name"),        _itemColumn, Qt::AlignLeft   );
  _location->addColumn(tr("Description"), -1,          Qt::AlignLeft   );
  _location->addColumn(tr("Netable"),     80,          Qt::AlignCenter );
  _location->addColumn(tr("Restricted"),  80,          Qt::AlignCenter );

  if (_privileges->check("MaintainLocations"))
  {
    connect(_location, SIGNAL(valid(bool)), _edit, SLOT(setEnabled(bool)));
    connect(_location, SIGNAL(valid(bool)), _delete, SLOT(setEnabled(bool)));
    connect(_location, SIGNAL(itemSelected(int)), _edit, SLOT(animateClick()));
  }
  else
  {
    _new->setEnabled(FALSE);
    connect(_location, SIGNAL(itemSelected(int)), _view, SLOT(animateClick()));
  }

  sFillList();
}

void locations::sNew()
{
  ParameterList params;
  params.append("mode", "new");

  if (_warehouse->isSelected())
    params.append("warehous_id", _warehouse->id());

  location newdlg(this, "", TRUE);
  newdlg.set(params);

  if (newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void locations::sEdit()
{
  ParameterList params;
  params.append("mode", "edit");
  params.append("location_id", _location->id());

  location newdlg(this, "", TRUE);
  newdlg.set(params);

  if (newdlg.exec() != XDialog::Rejected)
    sFillList();
}

void locations::sView()
{
  ParameterList params;
  params.append("mode", "view");
  params.append("location_id", _location->id());

  location newdlg(this, "", TRUE);
  newdlg.set(params);
  newdlg.exec();
}


void locations::sDelete()
{
  q.prepare("SELECT deleteLocation(:location_id) AS result;");
  q.bindValue(":location_id", _location->id());
  q.exec();
  if (q.first())
  {
    QString caption = tr("Cannot Delete Selected Location");
    QString msg;

    switch (q.value("result").toInt())
    {
      case -1:
        msg = tr("<p>There are one or more Item Sites that use the selected Location as their default Location. "
		 "You must assign the default Location for all Item Sites that use the selected Location before you may delete it.</p>" );
        break;

      case -2:
        msg = tr("<p>There is Inventory contained in the selected Location. "
		 "You must move all Inventory out of the selected Location and may then set its status to inactive.</p>" );
        break;

      case -3:
        msg = tr("<p>There are one or more undistributed Location records that have been posted against the selected Locations. "
		 "This probably indicates a system error.<br><br>"
		 "Please contact your Systems Adminstrator to have this resolved.</p>" );
        break;

      case -4:
        msg = tr("<p>The selected Location cannot be delete as there has been Inventory Transaction History posted against it. "
                 "You may edit the selected Location and set its status to inactive.</p>" );
        break;
    }
    if (! msg.isEmpty())
    {
      QMessageBox::critical(this, caption, msg);
      return;
    }

    sFillList();
  }

//  ToDo
}


void locations::sPrint()
{
  ParameterList params;
  _warehouse->appendValue(params);
  orReport report("WarehouseLocationMasterList", params);
  if (report.isValid())
    report.print();
  else
    report.reportError(this);
}


void locations::sFillList()
{
  QString sql( "SELECT location_id, warehous_code, formatLocationName(location_id), firstLine(location_descrip) AS locationname,"
               "       formatBoolYN(location_netable),"
               "       formatBoolYN(location_restrict) "
               "FROM location, warehous "
               "WHERE ( (location_warehous_id=warehous_id)" );

  if (_warehouse->isSelected())
    sql += " AND (warehous_id=:warehous_id)";

  sql += " ) "
         "ORDER BY warehous_code, locationname;";

  q.prepare(sql);
  q.bindValue(":warehous_id", _warehouse->id());
  q.exec();
  _location->populate(q);
}

