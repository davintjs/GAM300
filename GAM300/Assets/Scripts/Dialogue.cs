using BeanFactory;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

class Dialogue : Script
{
    public int state = 1;

    public void OnTriggerEnter(PhysicsComponent component)
    {
        if (GetTag(component) == "Player")
            DialogueManager.Instance.SetState(state);
    }

    public void OnTriggerExit(PhysicsComponent component)
    {
        if (GetTag(component) == "Player")
            DialogueManager.Instance.Reset();
    }
}