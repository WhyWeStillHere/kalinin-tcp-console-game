#include "event_tree.h"

template<typename  EventTypes>
typename EventManager<EventTypes>::TreeIterator
EventManager<EventTypes>::Begin() {
  return root_;
}

template<typename  EventTypes>
void EventManager<EventTypes>::AddEdge(EventTypes from, EventTypes to) {
  event_nodes_.at(from)->next_nodes.at(to) = event_nodes_.at(to);
}

template<typename  EventTypes>
template<typename FuncType>
void EventManager<EventTypes>::AddEvent(EventTypes type, FuncType event_handler) {
  if (event_nodes_.find(type) != event_nodes_.end()) {
    throw std::logic_error("This type already exist");
  }
  event_nodes_[type] = new HandlerEventNode<FuncType>(type, event_handler);
}

template <typename  EventTypes>

