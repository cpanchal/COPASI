# Copyright © 2005 by Pedro Mendes, Virginia Tech Intellectual
# Properties, Inc. and EML Research, gGmbH.
# All rights reserved.

import COPASI
import unittest
from types import *

class Test_CState(unittest.TestCase):
  def setUp(self):
    self.cstate=COPASI.CCopasiDataModel.GLOBAL.getModel().getInitialState()

  def test_getTime(self):
    time=self.cstate.getTime()
    self.assert_(type(time)==FloatType,"Error. The time value returned by CState.getTime() is not a double value.")

  def test_setTime(self):
    TIME=27.96
    self.cstate.setTime(TIME)
    self.assert_(self.cstate.getTime()==TIME)

  def test_getNumIndependent(self):
    result=self.cstate.getNumIndependent()
    self.assert_(type(result)==IntType)

  def test_getNumDependent(self):
    result=self.cstate.getNumDependent()
    self.assert_(type(result)==IntType)

  def test_getNumVariable(self):
    result=self.cstate.getNumVariable()
    self.assert_(type(result)==IntType)

  def test_getNumFixed(self):
    result=self.cstate.getNumFixed()
    self.assert_(type(result)==IntType)

  def test_isUpdateDependentRequired(self):
    result=self.cstate.isUpdateDependentRequired()
    self.assert_(type(result)==BooleanType)

  def test_setUpdateDependentRequired(self):
    UPDATE_REQUIRED=False
    self.cstate.setUpdateDependentRequired(UPDATE_REQUIRED)
    self.assert_(self.cstate.isUpdateDependentRequired()==UPDATE_REQUIRED)
    UPDATE_REQUIRED=True
    self.cstate.setUpdateDependentRequired(UPDATE_REQUIRED)
    self.assert_(self.cstate.isUpdateDependentRequired()==UPDATE_REQUIRED)

def suite():
  tests=[
          'test_getTime'
         ,'test_setTime'
         ,'test_getNumIndependent'
         ,'test_getNumDependent'
         ,'test_getNumVariable'
         ,'test_getNumFixed'
         ,'test_isUpdateDependentRequired'
         ,'test_setUpdateDependentRequired'
        ]
  return unittest.TestSuite(map(Test_CState,tests))

if(__name__ == '__main__'):
    unittest.TextTestRunner(verbosity=2).run(suite())

    
