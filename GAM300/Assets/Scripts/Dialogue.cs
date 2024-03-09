using BeanFactory;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Dialogue : Script
{
    public int dialogue_state = 1;
    bool done = false;

    public void OnTriggerEnter(PhysicsComponent component)
    {
        if (done) return;
        if (GetTag(component) == "Player")
        {
            DialogueManager.Instance.SetState(dialogue_state);
        }
    }

    public void OnTriggerExit(PhysicsComponent component)
    {
        if (done) return;
        if (GetTag(component) == "Player")
        {
            done = true;
        }
    }
}