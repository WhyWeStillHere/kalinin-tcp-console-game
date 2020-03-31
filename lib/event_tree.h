#pragma one

#include <string>
#include <vector>
#include <unordered_map>


template<typename  EventTypes>
class EventManager {
public:

  struct EventNode {
    explicit EventNode(EventTypes event_type) : event_type_(event_type) {
    }

    EventTypes event_type_;
    std::unordered_map<EventTypes, EventNode* > next_nodes_;
  };

  template< typename FuncType>
  struct HandlerEventNode : public EventNode {
    explicit HandlerEventNode(EventTypes event_type,
        FuncType event_handler)
        : EventNode(event_type),
          event_handler_(event_handler) {
    }

    FuncType event_handler_;
  };

  std::unordered_map<EventTypes, EventNode* >  event_nodes_;
  EventNode* root_ = nullptr;

public:
  EventManager() = default;

  template< typename FuncType>
  void AddEvent(EventTypes type, FuncType event_handler);

  void AddEdge(EventTypes from, EventTypes to);

  class TreeIterator {
    EventNode* current_event_;
    EventNode* prev_event_;

    void Next(EventTypes next_event);
    void Prev();
  };

  TreeIterator Begin();
};
