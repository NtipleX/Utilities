#include <iostream>
#include <functional>
#include <cstring>
#include <X11/XKBlib.h>     // -lX11
#include <X11/Xutil.h>
#include <vector>
#include <string>
#include <memory>

namespace KeyboardHelpers
{

    std::vector<std::string> readLayoutList();
    std::string getCurrentLayoutName(const std::vector<std::string>& layoutList);
    bool setCurrentLayout(const std::string& newLayout);
    std::string getCurrentHotkey();


    template<typename Obj, typename Fn>
    class Scope
    {
    public:
        Scope(Obj&& obj, Fn&& scopeExit)
              : object(std::move(obj))
              , callback(std::forward<Fn>(scopeExit))
        {}

        ~Scope()
        {
            callback(object);
        }

        Obj& get()
        {
            return object;
        }


    private:
        Obj object;
        Fn callback;
    };

    struct DisplayDeleter
    {
        void operator()(Display* display) const
        {
            XCloseDisplay(display);
        }
    };

    using DisplayPtr = std::unique_ptr<Display, DisplayDeleter>;

    using XkbDescRecScoped = Scope<XkbDescRec, std::function<void(XkbDescRec&)>>;

  
    std::vector<std::string> readLayoutList()
    {
        DisplayPtr display(XOpenDisplay(NULL));
        if(display == nullptr)
        {
            std::cout<<"Can't open display.";
            return std::vector<std::string>();
        }

        XkbDescRecScoped desc(XkbDescRec(), [](XkbDescRec& _desc)
        {
            XkbFreeControls(&_desc, XkbGroupsWrapMask, True);
            XkbFreeNames(&_desc, XkbGroupNamesMask, True);
        });

        desc.get().device_spec = XkbUseCoreKbd;
        Status status = XkbGetControls(display.get(), XkbGroupsWrapMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetControls error: "<<status;
            return std::vector<std::string>();
        }

        status = XkbGetNames(display.get(), XkbGroupNamesMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetControls error: "<<status;
            return std::vector<std::string>();
        }

        std::vector<std::string> layoutList;
        for(int i = 0; i < desc.get().ctrls->num_groups; ++i)
        {
            std::string name = XGetAtomName(display.get(), desc.get().names->groups[i]);
            layoutList.emplace_back(name);
        }

        return layoutList;
    }

    std::string getCurrentLayoutName(const std::vector<std::string>& layoutList)
    {
        DisplayPtr display(XOpenDisplay(NULL));
        if(display == nullptr)
        {
            std::cout<<"Can't open display."<<std::endl;
            return std::string();


        XkbStateRec xkbState;
        Status status = XkbGetState(display.get(), XkbUseCoreKbd, &xkbState);
        if(status != Success)
        {
            std::cout<<"XkbGetState error: "<<status<<std::endl;
            return std::string();
        }

        std::string name = layoutList[xkbState.locked_group];

        return name;
    }

    bool setCurrentLayout(const std::string& newLayout)
    {
        DisplayPtr display(XOpenDisplay(NULL));
        if(display == nullptr)
        {
            std::cout<<"Can't open display."<<std::endl;
            return false;
        }

        XkbDescRecScoped desc(XkbDescRec(), [](XkbDescRec& _desc)
        {
            XkbFreeControls(&_desc, XkbGroupsWrapMask, True);
            XkbFreeNames(&_desc, XkbGroupNamesMask, True);
        });

        desc.get().device_spec = XkbUseCoreKbd;
        Status status = XkbGetControls(display.get(), XkbGroupsWrapMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetControls error: "<<status;
            return false;
        }

        status = XkbGetNames(display.get(), XkbSymbolsNameMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetNames, XkbSymbolsNameMask error: "<<status<<std::endl;
            return false;
        }

        status = XkbGetNames(display.get(), XkbGroupNamesMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetNames, XkbGroupNamesMask error: "<<status;
            return false;
        }

        Atom* groupSource = desc.get().names->groups;
        size_t groupCount = 0;
        if (desc.get().ctrls != nullptr)
        {
            groupCount = desc.get().ctrls->num_groups;
        }
        else
        {
            groupCount = 0;
            while (groupCount < XkbNumKbdGroups &&
                   groupSource[groupCount] != 0)
            {
                groupCount++;
            }
        }


        Atom* tmpGroupSource = desc.get().names->groups;
        Atom curGroupAtom;
        std::string groupName;
        for (size_t i = 0; i < groupCount; i++)
        {
            if ((curGroupAtom = tmpGroupSource[i]) != None)
            {
                std::string groupNameC = XGetAtomName(display.get(), curGroupAtom);
                if (groupNameC.empty())
                    continue;
                else
                {
                    groupName =  groupNameC;

                    if (strncmp(newLayout.data(), groupName.data(), 7) == 0)
                    {
                        XkbLockGroup(display.get(), XkbUseCoreKbd, i);
                    }
                }
            }
        }
        return true;
    }

    std::string getCurrentHotkey()
    {
        DisplayPtr display(XOpenDisplay(NULL));
        if(display == nullptr)
        {
            std::cout<<"Can't open display."<<std::endl;
            return std::string();
        }

        XkbDescRecScoped desc(XkbDescRec(), [](XkbDescRec& _desc)
        {
            XkbFreeControls(&_desc, XkbGroupsWrapMask, True);
            XkbFreeNames(&_desc, XkbGroupNamesMask, True);
        });

        desc.get().device_spec = XkbUseCoreKbd;
        Status status = XkbGetNames(display.get(), XkbSymbolsNameMask, &desc.get());
        if(status != Success)
        {
            std::cout<<"XkbGetNames error: "<<status<<std::endl;
            return std::string();
        }

        Atom symName = desc.get().names->symbols;

        std::string layoutString = XGetAtomName(display.get(), symName);

        const std::string searchStr = "(evdev)";
        std::size_t pos = layoutString.rfind(searchStr);
        if(pos != std::string::npos)
           pos = pos + searchStr.size() + 1;
        else
           return std::string();


        if(pos < layoutString.size())
            layoutString = layoutString.substr(pos);
        else
            return std::string();

        std::size_t length = layoutString.find('+');
        length = (length == std::string::npos) ? layoutString.size() : length;

        return layoutString.substr(0, length);
    }

}
