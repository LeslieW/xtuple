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
 * (c) 1999-2007 OpenMFG, LLC, d/b/a xTuple. All Rights Reserved. 
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
 * Copyright (c) 1999-2007 by OpenMFG, LLC, d/b/a xTuple
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

#ifndef RETURNAUTHORIZATION_H
#define RETURNAUTHORIZATION_H

#include "OpenMFGGUIClient.h"
#include <QMainWindow>
#include <parameter.h>

#include "ui_returnAuthorization.h"

#include "taxCache.h"

class returnAuthorization : public QMainWindow, public Ui::returnAuthorization
{
    Q_OBJECT

public:
    returnAuthorization(QWidget* parent = 0, const char* name = 0, Qt::WFlags fl = Qt::WType_TopLevel);
    ~returnAuthorization();

    virtual void setNumber();

public slots:
    virtual SetResponse set( const ParameterList & pParams );
    virtual void recalculateTax();
    virtual bool sSave();
	virtual void sSaveClick();
    virtual void sShipToList();
    virtual void sParseShipToNumber();
    virtual void populateShipto( int pShiptoid );
    virtual void sPopulateCustomerInfo();
    virtual void sCheckAuthorizationNumber();
	virtual void sClearShiptoNumber();
    virtual void sCopyToShipto();
//	virtual void sCustChanged();
    virtual void sNew();
    virtual void sEdit();
	virtual void sView();
	virtual void sAction();
    virtual void sDelete();
    virtual void sFillList();
    virtual void sCalculateSubtotal();
    virtual void sCalculateTotal();
    virtual void populate();
    virtual void closeEvent( QCloseEvent * pEvent );
    virtual void sFreightChanged();
    virtual void sTaxAuthChanged();
    virtual void sTaxDetail();
	virtual void sOrigSoChanged();
	virtual void sDispositionChanged();
    virtual void sCreditByChanged();
	virtual void sAuthorizeLine();
	virtual void sClearAuthorization();
	virtual void sAuthorizeAll();
    virtual void sEnterReceipt();
    virtual void sReceiveAll();
    virtual void sHandleEnterReceipt(bool);
	virtual void sHandleAction();
    virtual void sHandleSalesOrderEvent( int pSoheadid, bool );
    virtual void sRefund();
    virtual void sPostReceipts();

protected slots:
    virtual void languageChange();

private:
    int _mode;
    int _raheadid;
    int _shiptoid;
    int	_custtaxauthid;
    bool _ffBillto;
    bool _ffShipto;
    bool _ignoreShiptoSignals;
	bool _ignoreSoSignals;
    double _subtotalCache;
    int _taxauthidCache;
    int _taxcurrid;

    taxCache _taxCache;
	char *dispositionTypes[];
    char *creditMethods[];
};

#endif // RETURNAUTHORIZATION_H
