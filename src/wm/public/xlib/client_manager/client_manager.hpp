//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_CLIENT_MANAGER_HPP
#define FLOW_WM_CLIENT_MANAGER_HPP
#include "../client/client.hpp"

namespace flow::X11
{
	class ClientManager
	{
	public:
		ClientManager() = default;

		Client* selected = nullptr;

		void AddClient(Client* client);
		bool Exists(Client* client);
		bool Exists(Window window);
		[[nodiscard]] unsigned int GetCount() const;
		Client* GetClient(Window window);
		void RemoveClient(Client* client);
		void RemoveClient(Window window);

		Client* GetFirst();
		Client* GetLast();
		void SetFirst(Client* client);
		void SetLast(Client* client);

		void Focus(X11::Client* client, bool fromButtonPress = false);
		void UnFocus(X11::Client* client, int set_focus);
		void Manage(Window window, XWindowAttributes* wa);
		void UnManage(X11::Client* client, int destroyed);
		void Frame(X11::Client* client);
		void UnFrame(X11::Client* client);
		void Resize(X11::Client* client, int x, int y, int w, int h, int interact);
		void ReparentToBase(Client* client);
		static long GetState(Window window);
		void KillClient(Client* client);

		X11::Client* WindowToClient(Window w);
		bool DontTouchWindow(Window w);

		bool CheckAtom(Window window, Atom big_atom, Atom small_atom);
		void * GetAtom(Window window, Atom atom, unsigned long *items);
		X11::Client* GetClientFromFrame(Window window);
	private:
		Client* first = nullptr;
		Client* last = nullptr;
		unsigned int count = 0;
	};
}

#endif //FLOW_WM_CLIENT_MANAGER_HPP
