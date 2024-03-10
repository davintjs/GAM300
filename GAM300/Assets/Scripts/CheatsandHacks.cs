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
        if (Input.GetKeyDown(KeyCode.D5))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp1.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.D6))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp2.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.D7))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp3.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.D8))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp4.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.D9))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp5.localPosition);
        }

        if (Input.GetKeyDown(KeyCode.D0))
        {
            ThirdPersonController.instance.player.localPosition = new vec3(tp6.localPosition);
        }
    }
}
