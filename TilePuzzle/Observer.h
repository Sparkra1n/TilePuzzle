#pragma once

#include <SDL.h>
#include "Events.h"
class Observer
{
public:
    Observer() = default;
    virtual ~Observer() = default;
    //void setLowerObserver(Observer* observer) { m_lowerObserver = observer; }

    //template<typename... Args>
    //auto handleEvent(Event event, Args&&... args) -> decltype(auto)
    //{
	   // switch (event)
	   // {
	   // case Event::Unknown:
    //        std::cout << "Unknown Case\n";
    //        break;
	   // default:
    //        return m_lowerObserver->handleEvent(std::forward<Args>(args)...);
	   // }
    //}

protected:
	Observer* m_lowerObserver{};
};



