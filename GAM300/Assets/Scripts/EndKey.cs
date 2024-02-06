using BeanFactory;
using System;
using System.Collections.Generic;
using System.Diagnostics.SymbolStore;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class EndKey : Script
{
    public GameObject mesh;

    void Start()
    {
        
    }

    void Update()
    {

    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            AudioManager.instance.keyCollected.Play();
            Destroy(mesh);
        }
    }
}