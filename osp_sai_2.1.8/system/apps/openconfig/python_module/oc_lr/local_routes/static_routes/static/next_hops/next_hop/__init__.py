
from operator import attrgetter
from pyangbind.lib.yangtypes import RestrictedPrecisionDecimalType
from pyangbind.lib.yangtypes import RestrictedClassType
from pyangbind.lib.yangtypes import TypedListType
from pyangbind.lib.yangtypes import YANGBool
from pyangbind.lib.yangtypes import YANGListType
from pyangbind.lib.yangtypes import YANGDynClass
from pyangbind.lib.yangtypes import ReferenceType
from pyangbind.lib.base import PybindBase
from decimal import Decimal
from bitarray import bitarray
import six

# PY3 support of some PY2 keywords (needs improved)
if six.PY3:
  import builtins as __builtin__
  long = int
  unicode = str
elif six.PY2:
  import __builtin__

from . import config
from . import state
from . import interface_ref
class next_hop(PybindBase):
  """
  This class was auto-generated by the PythonClass plugin for PYANG
  from YANG module openconfig-local-routing - based on the path /local-routes/static-routes/static/next-hops/next-hop. Each member element of
  the container is represented as a class variable - with a specific
  YANG type.

  YANG Description: A list of next-hops to be utilised for the static
route being specified.
  """
  __slots__ = ('_pybind_generated_by', '_path_helper', '_yang_name', '_extmethods', '__index','__config','__state','__interface_ref',)

  _yang_name = 'next-hop'

  _pybind_generated_by = 'container'

  def __init__(self, *args, **kwargs):

    self._path_helper = False

    self._extmethods = False
    self.__index = YANGDynClass(base=unicode, is_leaf=True, yang_name="index", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, is_keyval=True, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='leafref', is_config=True)
    self.__state = YANGDynClass(base=state.state, is_container='container', yang_name="state", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)
    self.__config = YANGDynClass(base=config.config, is_container='container', yang_name="config", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)
    self.__interface_ref = YANGDynClass(base=interface_ref.interface_ref, is_container='container', yang_name="interface-ref", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)

    load = kwargs.pop("load", None)
    if args:
      if len(args) > 1:
        raise TypeError("cannot create a YANG container with >1 argument")
      all_attr = True
      for e in self._pyangbind_elements:
        if not hasattr(args[0], e):
          all_attr = False
          break
      if not all_attr:
        raise ValueError("Supplied object did not have the correct attributes")
      for e in self._pyangbind_elements:
        nobj = getattr(args[0], e)
        if nobj._changed() is False:
          continue
        setmethod = getattr(self, "_set_%s" % e)
        if load is None:
          setmethod(getattr(args[0], e))
        else:
          setmethod(getattr(args[0], e), load=load)

  def _path(self):
    if hasattr(self, "_parent"):
      return self._parent._path()+[self._yang_name]
    else:
      return [u'local-routes', u'static-routes', u'static', u'next-hops', u'next-hop']

  def _get_index(self):
    """
    Getter method for index, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/index (leafref)

    YANG Description: A reference to the index of the current next-hop.
The index is intended to be a user-specified value
which can be used to reference the next-hop in
question, without any other semantics being
assigned to it.
    """
    return self.__index
      
  def _set_index(self, v, load=False):
    """
    Setter method for index, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/index (leafref)
    If this variable is read-only (config: false) in the
    source YANG file, then _set_index is considered as a private
    method. Backends looking to populate this variable should
    do so via calling thisObj._set_index() directly.

    YANG Description: A reference to the index of the current next-hop.
The index is intended to be a user-specified value
which can be used to reference the next-hop in
question, without any other semantics being
assigned to it.
    """
    parent = getattr(self, "_parent", None)
    if parent is not None and load is False:
      raise AttributeError("Cannot set keys directly when" +
                             " within an instantiated list")

    if hasattr(v, "_utype"):
      v = v._utype(v)
    try:
      t = YANGDynClass(v,base=unicode, is_leaf=True, yang_name="index", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, is_keyval=True, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='leafref', is_config=True)
    except (TypeError, ValueError):
      raise ValueError({
          'error-string': """index must be of a type compatible with leafref""",
          'defined-type': "leafref",
          'generated-type': """YANGDynClass(base=unicode, is_leaf=True, yang_name="index", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, is_keyval=True, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='leafref', is_config=True)""",
        })

    self.__index = t
    if hasattr(self, '_set'):
      self._set()

  def _unset_index(self):
    self.__index = YANGDynClass(base=unicode, is_leaf=True, yang_name="index", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, is_keyval=True, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='leafref', is_config=True)


  def _get_config(self):
    """
    Getter method for config, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/config (container)

    YANG Description: Configuration parameters relating to the next-hop
entry
    """
    return self.__config
      
  def _set_config(self, v, load=False):
    """
    Setter method for config, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/config (container)
    If this variable is read-only (config: false) in the
    source YANG file, then _set_config is considered as a private
    method. Backends looking to populate this variable should
    do so via calling thisObj._set_config() directly.

    YANG Description: Configuration parameters relating to the next-hop
entry
    """
    if hasattr(v, "_utype"):
      v = v._utype(v)
    try:
      t = YANGDynClass(v,base=config.config, is_container='container', yang_name="config", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)
    except (TypeError, ValueError):
      raise ValueError({
          'error-string': """config must be of a type compatible with container""",
          'defined-type': "container",
          'generated-type': """YANGDynClass(base=config.config, is_container='container', yang_name="config", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)""",
        })

    self.__config = t
    if hasattr(self, '_set'):
      self._set()

  def _unset_config(self):
    self.__config = YANGDynClass(base=config.config, is_container='container', yang_name="config", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)


  def _get_state(self):
    """
    Getter method for state, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/state (container)

    YANG Description: Operational state parameters relating to the
next-hop entry
    """
    return self.__state
      
  def _set_state(self, v, load=False):
    """
    Setter method for state, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/state (container)
    If this variable is read-only (config: false) in the
    source YANG file, then _set_state is considered as a private
    method. Backends looking to populate this variable should
    do so via calling thisObj._set_state() directly.

    YANG Description: Operational state parameters relating to the
next-hop entry
    """
    if hasattr(v, "_utype"):
      v = v._utype(v)
    try:
      t = YANGDynClass(v,base=state.state, is_container='container', yang_name="state", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)
    except (TypeError, ValueError):
      raise ValueError({
          'error-string': """state must be of a type compatible with container""",
          'defined-type': "container",
          'generated-type': """YANGDynClass(base=state.state, is_container='container', yang_name="state", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)""",
        })

    self.__state = t
    if hasattr(self, '_set'):
      self._set()

  def _unset_state(self):
    self.__state = YANGDynClass(base=state.state, is_container='container', yang_name="state", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)


  def _get_interface_ref(self):
    """
    Getter method for interface_ref, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/interface_ref (container)

    YANG Description: Reference to an interface or subinterface
    """
    return self.__interface_ref
      
  def _set_interface_ref(self, v, load=False):
    """
    Setter method for interface_ref, mapped from YANG variable /local_routes/static_routes/static/next_hops/next_hop/interface_ref (container)
    If this variable is read-only (config: false) in the
    source YANG file, then _set_interface_ref is considered as a private
    method. Backends looking to populate this variable should
    do so via calling thisObj._set_interface_ref() directly.

    YANG Description: Reference to an interface or subinterface
    """
    if hasattr(v, "_utype"):
      v = v._utype(v)
    try:
      t = YANGDynClass(v,base=interface_ref.interface_ref, is_container='container', yang_name="interface-ref", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)
    except (TypeError, ValueError):
      raise ValueError({
          'error-string': """interface_ref must be of a type compatible with container""",
          'defined-type': "container",
          'generated-type': """YANGDynClass(base=interface_ref.interface_ref, is_container='container', yang_name="interface-ref", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)""",
        })

    self.__interface_ref = t
    if hasattr(self, '_set'):
      self._set()

  def _unset_interface_ref(self):
    self.__interface_ref = YANGDynClass(base=interface_ref.interface_ref, is_container='container', yang_name="interface-ref", parent=self, path_helper=self._path_helper, extmethods=self._extmethods, register_paths=True, extensions=None, namespace='http://openconfig.net/yang/local-routing', defining_module='openconfig-local-routing', yang_type='container', is_config=True)

  index = __builtin__.property(_get_index, _set_index)
  config = __builtin__.property(_get_config, _set_config)
  state = __builtin__.property(_get_state, _set_state)
  interface_ref = __builtin__.property(_get_interface_ref, _set_interface_ref)


  _pyangbind_elements = {'index': index, 'config': config, 'state': state, 'interface_ref': interface_ref, }


