
/*
Software License :

Copyright (c) 2007-2009, The Open Effects Association Ltd.  All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.
    * Neither the name The Open Effects Association Ltd, nor the names of its 
      contributors may be used to endorse or promote products derived from this
      software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef OFXH_PARAM_H
#define OFXH_PARAM_H
#include "ofxhPropertySuite.h"
#include "ofxhAttribute.h"
#include "ofxCore.h"

#include <string>
#include <map>
#include <list>
#include <cstdarg>
#include <cassert>

namespace OFX {

  namespace Host {

    namespace Param {

      /// fetch the param suite
      void *GetSuite(int version);

      /// is this a standard BaseType
      bool isStandardType(const std::string &BaseType);

      class Instance;
      class SetInstance;

      /// base class to the param set instance and param set descriptor
      class BaseSet {
      public:
        virtual ~BaseSet();

        /// obtain a handle on this set for passing to the C api
        OfxParamSetHandle getParamSetHandle() const;

        /// get the property handle that lives with the set
        /// The plugin descriptor/instance that derives from
        /// this will provide this.
        virtual Property::Set &getParamSetProps() = 0;
      };

      /// base class for all params
      class Base : public Attribute {

      private:
        Base();
      protected:
        std::string   _paramType;
      public:
        Base(const std::string &name, const std::string &paramType);
        Base(const std::string &name, const std::string &paramType, const Property::Set &properties);
        virtual ~Base();

        /// grab a handle on the parameter for passing to the C API
        OfxParamHandle getHandle() const;

        virtual bool verifyMagic() { return true; }

        /// grab a handle on the properties of this parameter for the C api
        OfxPropertySetHandle getPropHandle() const;

        const std::string &getParamType() const;

        const std::string &getName() const;

        const std::string &getParentName() const;

        const std::string &getLabel() const;

        const std::string &getShortLabel() const;

        const std::string &getLongLabel() const;

        const std::string &getScriptName() const;

        const std::string &getDoubleType() const;

        const std::string &getCacheInvalidation() const;

        const std::string &getHint() const;

        bool getEnabled() const;

        bool getCanUndo() const;

        bool getSecret() const;

        bool getEvaluateOnChange() const;

        bool getCanAnimate() const;
      };

      /// the Descriptor of a plugin parameter
      class Descriptor : public Base {
        Descriptor();

      public:
        /// make a parameter, with the given type and name
        Descriptor(const std::string &type, const std::string &name);

        /// add standard param props, will call the below
        void addStandardParamProps(const std::string &type);

        /// add standard properties to a params that can take an interact
        void addInteractParamProps(const std::string &type);

        /// add standard properties to a value holding param
        void addValueParamProps(const std::string &type, Property::TypeEnum valueType, int dim);

        /// add standard properties to a value holding param
        void addNumericParamProps(const std::string &type, Property::TypeEnum valueType, int dim);
      };

      /// A set of parameters
      ///
      /// As we are the owning object we delete the params inside ourselves. It was tempting
      /// to make params autoref objects and have shared ownership with the client code
      /// but that adds complexity for no strong gain.
      class SetInstance : public BaseSet {
      protected:
        std::map<std::string, Instance*> _params;        ///< params by name
        std::list<Instance *>            _paramList;     ///< params list

      public :
        /// ctor
        ///
        /// The propery set being passed in belongs to the owning
        /// plugin instance.
        explicit SetInstance();

        /// dtor.
        virtual ~SetInstance();

        /// get the params
        inline const std::map<std::string, Instance*> &getParams() const;
        inline std::map<std::string, Instance*> &getParams();

        /// get the params
        inline const std::list<Instance*> &getParamList() const;
        inline std::list<Instance*> &getParamList();

        // get the param
        Instance* getParam(std::string name) {
          std::map<std::string,Instance*>::iterator it = _params.find(name);
          if(it!=_params.end())
            return it->second;
          else
            return 0;
        }

        /// The inheriting plugin instance needs to set this up to deal with
        /// plug-ins changing their own values.
        virtual void paramChangedByPlugin(Param::Instance *param) = 0;

        /// add a param
        virtual OfxStatus addParam(const std::string& name, Instance* instance);

        /// make a parameter instance
        ///
        /// Client host code needs to implement this
        virtual Instance* newParam(const std::string& name, Descriptor& Descriptor, SetInstance * setInstance) = 0;

        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
        ///
        /// Client host code needs to implement this
        virtual OfxStatus editBegin(const std::string& name) = 0;

        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
        ///
        /// Client host code needs to implement this
        virtual OfxStatus editEnd() = 0;
      };

      /// a set of parameters
      class SetDescriptor : public BaseSet {
        std::map<std::string, Descriptor*> _paramMap;
        std::list<Descriptor *> _paramList;
        
        /// CC doesn't exist
        SetDescriptor(const SetDescriptor &);

      public:
        /// default ctor
        SetDescriptor();

        /// dtor
        virtual ~SetDescriptor();

        /// get the map of params
        const std::map<std::string, Descriptor*> &getParams() const;
        std::map<std::string, Descriptor*> &getParams();

        /// get the list of params
        const std::list<Descriptor *> &getParamList() const;

        /// define a param
        virtual Descriptor *paramDefine(const char *paramType,
                                        const char *name);

        /// add a param in
        virtual void addParam(const std::string &name, Descriptor *p);
      };

      /// the description of a plugin parameter
      class Instance : public Base, private Property::NotifyHook {
        Instance();  
      protected:
        SetInstance*  _paramSetInstance;
        Instance*     _parentInstance;
      public:
        virtual ~Instance();

        /// make a parameter, with the given type and name
        explicit Instance( Descriptor& descriptor, Param::SetInstance & setInstance );

        //        OfxStatus instanceChangedAction(std::string why,
        //                                        OfxTime     time,
        //                                        double      renderScaleX,
        //                                        double      renderScaleY);

        // get the param instance
        OFX::Host::Param::SetInstance* getParamSetInstance() { return _paramSetInstance; }
        void setParamSetInstance( OFX::Host::Param::SetInstance *instance ) { _paramSetInstance = instance; }

        // set/get parent instance
        void setParentInstance( Instance* instance );
        Instance* getParentInstance();

        // copy one parameter to another
        virtual OfxStatus copy( const Instance &instance, OfxTime offset );

        // copy one parameter to another, with a range
        virtual OfxStatus copy( const Instance &instance, OfxTime offset, OfxRangeD range );

        // callback which should set enabled state as appropriate
        virtual void setEnabled();

        // callback which should set secret state as appropriate
        virtual void setSecret();

        /// callback which should update label
        virtual void setLabel();

        /// callback which should set 
        virtual void setDisplayRange();

        // va list calls below turn the var args (oh what a mistake)
        // suite functions into virtual function calls on instances
        // they are not to be overridden by host implementors by
        // by the various typeed param instances so that they can
        // deconstruct the var args lists

        /// get a value, implemented by instances to deconstruct var args
        virtual OfxStatus getV( va_list arg );

        /// get a value, implemented by instances to deconstruct var args
        virtual OfxStatus getV( OfxTime time, va_list arg );

        /// set a value, implemented by instances to deconstruct var args
        virtual OfxStatus setV( va_list arg );

        /// key a value, implemented by instances to deconstruct var args
        virtual OfxStatus setV( OfxTime time, va_list arg );

        /// derive a value, implemented by instances to deconstruct var args
        virtual OfxStatus deriveV( OfxTime time, va_list arg );

        /// integrate a value, implemented by instances to deconstruct var args
        virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg );

        /// overridden from Property::NotifyHook
        virtual void notify( const std::string &name, bool single, int num ) OFX_EXCEPTION_SPEC;
      };

      class KeyframeParam {
      public:
        virtual OfxStatus getNumKeys(unsigned int &nKeys) const ;
        virtual OfxStatus getKeyTime(int nth, OfxTime& time) const ;
        virtual OfxStatus getKeyIndex(OfxTime time, int direction, int & index) const ;
        virtual OfxStatus deleteKey(OfxTime time) ;
        virtual OfxStatus deleteAllKeys() ;

        virtual ~KeyframeParam() {
        }
      };

      template <class T, int DIM>
      class MultiDimParam : public Instance, public KeyframeParam  {
	  public:
            typedef T Type;
            typedef typename T::BaseType BaseType;
          protected:
            std::vector<T*> _controls;
          public:
            MultiDimParam(Descriptor& descriptor, Param::SetInstance & setInstance): Instance(descriptor, setInstance) {
            }

            virtual ~MultiDimParam() {
                for( typename std::vector<T*>::iterator it = _controls.begin();
                    it != _controls.end(); ++it ) {
                    delete *it;
                }
            }

            // Deriving implementatation needs to overide these
            inline virtual OfxStatus get( BaseType & dst, const size_t & index ) {
                assert( _controls.size() > index );
                return _controls[index]->get( dst );
            }
            inline virtual OfxStatus get(OfxTime time, BaseType & dst, const size_t & index) {
                assert( _controls.size() > index );
                return _controls[index]->get( time, dst );
            }
            inline virtual OfxStatus set(const BaseType & value, const size_t & index) {
                assert( _controls.size() > index );
                return _controls[index]->set(value);
            }
            inline virtual OfxStatus set(OfxTime time, const BaseType & value, const size_t & index) {
                assert( _controls.size() > index );
                return _controls[index]->set( time, value );
            }

            // derived class does not need to implement, default is an approximation
            inline virtual OfxStatus derive(OfxTime time, BaseType & dst, const size_t & index) {
                assert( _controls.size() > index );
                return _controls[index]->derive( time, dst );
            }
            inline virtual OfxStatus integrate(OfxTime time1, OfxTime time2, BaseType & dst, const size_t & index) {
                assert( _controls.size() > index );
                return _controls[index]->integrate( time1, time2, dst );
            }

            /// implementation of var args function
            virtual OfxStatus getV(va_list arg) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType *v = va_arg( arg, BaseType* );
                    assert( v );
                    st |= _controls[i]->get( *v );
                }
                return st;
            }

            /// implementation of var args function
            virtual OfxStatus getV( OfxTime time, va_list arg ) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType *v = va_arg( arg, BaseType* );
                    st |= _controls[i]->get( time, *v );
                }
                return st;
            }

            /// implementation of var args function
            virtual OfxStatus setV( va_list arg ) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType v = va_arg( arg, BaseType );
                    st |= _controls[i]->set( v );
                }
                return st;
            }

            /// implementation of var args function
            virtual OfxStatus setV(OfxTime time, va_list arg) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType v = va_arg( arg, BaseType );
                    st |= _controls[i]->set( time, v );
                }
                return st;
            }

            /// implementation of var args function
            virtual OfxStatus deriveV(OfxTime time, va_list arg) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType *v = va_arg( arg, BaseType* );
                    st  |= _controls[i]->derive( time, *v );
                }
                return st;
            }

            /// implementation of var args function
            virtual OfxStatus integrateV( OfxTime time1, OfxTime time2, va_list arg ) {
                OfxStatus st = kOfxStatOK;
                for( int i = 0; i < DIM; ++i ) {
                    BaseType *v = va_arg( arg, BaseType* );
                    assert( v );
                    st |= _controls[i]->integrate( time1, time2, *v );
                }
                return st;
            }
      };

      class GroupInstance : public Instance, public SetInstance {
      public:
        GroupInstance( Descriptor& descriptor, Param::SetInstance & setInstance ) : Instance(descriptor, setInstance) {}

        /// setChildrens have to clone each source instance recursively
        void setChildrens( const Param::SetInstance * childrens );
        Param::SetInstance *getChildrens() const;
        void addChildren( Instance * children );

        Property::Set &getParamSetProps() {
            return _paramSetInstance->getParamSetProps();
        }

        /// The inheriting plugin instance needs to set this up to deal with
        /// plug-ins changing their own values.
        virtual void paramChangedByPlugin(Param::Instance *param) {
            _paramSetInstance->paramChangedByPlugin( param );
        }

        virtual Instance* newParam(const std::string& name, Descriptor& Descriptor, SetInstance * setInstance) {
            return _paramSetInstance->newParam( name, Descriptor, setInstance );
        }

        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditBegin
        virtual OfxStatus editBegin(const std::string& name) {
            return _paramSetInstance->editBegin( name );
        }


        /// Triggered when the plug-in calls OfxParameterSuiteV1::paramEditEnd
        virtual OfxStatus editEnd() {
            return _paramSetInstance->editEnd();
        }
      };

      class PageInstance : public Instance {
      public:
        PageInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}
        const std::map<int,Param::Instance*> &getChildren() const;
      protected :
        mutable std::map<int,Param::Instance*> _children; // if set in a notify hook, this need not be mutable
      };

      class IntegerInstance : public Instance, public KeyframeParam {
      public:
        typedef int BaseType;
        IntegerInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}

        // Deriving implementatation needs to overide these 
        virtual OfxStatus get(int&) = 0;
        virtual OfxStatus get(OfxTime time, int&) = 0;
        virtual OfxStatus set(int) = 0;
        virtual OfxStatus set(OfxTime time, int) = 0;

        // probably derived class does not need to implement, default is an approximation
        virtual OfxStatus derive(OfxTime time, int&);
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, int&);

        /// implementation of var args function
        virtual OfxStatus getV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus getV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus deriveV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus integrateV(OfxTime time1, OfxTime time2, va_list arg);
      };

      class ChoiceInstance : public Instance, public KeyframeParam {
      public:
        ChoiceInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}

        // Deriving implementatation needs to overide these 
        virtual OfxStatus get(int&) = 0;
        virtual OfxStatus get(OfxTime time, int&) = 0;
        virtual OfxStatus set(int) = 0;
        virtual OfxStatus set(OfxTime time, int) = 0;

        /// implementation of var args function
        virtual OfxStatus getV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus getV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(OfxTime time, va_list arg);
      };

      class DoubleInstance : public Instance, public KeyframeParam {
      public:
        typedef double BaseType;
        DoubleInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}

        // Deriving implementatation needs to overide these 
        virtual OfxStatus get(double&) = 0;
        virtual OfxStatus get(OfxTime time, double&) = 0;
        virtual OfxStatus set(double) = 0;
        virtual OfxStatus set(OfxTime time, double) = 0;
        virtual OfxStatus derive(OfxTime time, double&) = 0;
        virtual OfxStatus integrate(OfxTime time1, OfxTime time2, double&) = 0;

        /// implementation of var args function
        virtual OfxStatus getV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus getV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus deriveV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus integrateV(OfxTime time1, OfxTime time2, va_list arg);
      };

      class BooleanInstance : public Instance, public KeyframeParam {
      public:
        typedef bool BaseType;
        BooleanInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}

        // Deriving implementatation needs to overide these
        virtual OfxStatus get(bool&) = 0;
        virtual OfxStatus get(OfxTime time, bool&) = 0;
        virtual OfxStatus set(bool) = 0;
        virtual OfxStatus set(OfxTime time, bool) = 0;

        /// implementation of var args function
        virtual OfxStatus getV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus getV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(OfxTime time, va_list arg);
      };

      class StringInstance : public Instance, public KeyframeParam {
        std::string _returnValue; ///< location to hold temporary return value. Should delegate this to implementation!!!
      public:
        typedef std::string BaseType;
        StringInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}

        virtual OfxStatus get(std::string &) = 0;
        virtual OfxStatus get(OfxTime time, std::string &) = 0;
        virtual OfxStatus set(const char*) = 0;
        virtual OfxStatus set(OfxTime time, const char*) = 0;

        /// implementation of var args function
        virtual OfxStatus getV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus getV(OfxTime time, va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(va_list arg);

        /// implementation of var args function
        virtual OfxStatus setV(OfxTime time, va_list arg);
      };

      class CustomInstance : public StringInstance {
      public:
        CustomInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : StringInstance(descriptor, setInstance) {}
      };

      class PushbuttonInstance : public Instance, public KeyframeParam {
      public:
        PushbuttonInstance(Descriptor& descriptor, Param::SetInstance & setInstance) : Instance(descriptor, setInstance) {}
      };
    }
  }
}

#endif // OFXH_PARAM_H
