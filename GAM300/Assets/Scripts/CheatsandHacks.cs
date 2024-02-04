using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class CheatsandHacks : Script
{
    public Transform tp1, tp2, tp3, tp4, tp5, tp6;

    void Update()
    {
        TeleportCheat();
    }

    //Teleport Cheats
    void TeleportCheat()
    {
        if (Input.GetKeyDown(KeyCode.KP1))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp1.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.KP2))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp2.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.KP3))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp3.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.KP4))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp4.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.KP5))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp5.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.KP6))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp6.localPosition);
        }
    }
}
