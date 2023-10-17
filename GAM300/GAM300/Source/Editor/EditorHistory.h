#ifndef EDITORHISTORY_H
#define EDITORHISTORY_H

#include "Precompiled.h"
#include "Scene/Object.h"

using Component = Object*;

//struct for each undo/redo change
struct Change {

	Change() {
		component = nullptr;
	}

	Change(Component comp, std::string prop) : component(comp), property(prop) {}

	Component component;
	std::string property; //name of property that changed

	property::data previousValue;
	property::data newValue;
};

using History = std::stack<Change>;

class HistoryManager {
	public:
		bool UndoStackEmpty();
		bool RedoStackEmpty();
		void ClearRedoStack();

		bool UndoChange();

		//redo will be populated with undo changes
		bool RedoChange();

		//change a value and add the change to the undo buffer
		template <typename T>
		void SetPropertyValue(Change& change, T& var, T& value) {
			change.previousValue = var;
			var = value;
			change.newValue = var;
			//push this change onto the undo stack
			UndoStack.push(change);

			//clear redo when new changes are made
			ClearRedoStack();
		}
	private: 
		History UndoStack;
		History RedoStack;
};

#endif //EDITORHISTORY_H