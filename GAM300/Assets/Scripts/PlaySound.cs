using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

class PlaySound : Script
{
    public AudioSource audioSource;

    void OnTriggerEnter(PhysicsComponent rb)
    {
        if (audioSource == null)
            return;
        if (GetTag(rb) == "Player")
            audioSource.Play();
    }
}
