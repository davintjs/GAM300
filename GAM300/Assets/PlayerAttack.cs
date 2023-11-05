using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class PlayerAttack : Script
{
    public Transform player;
    public Transform orient;
    public float distance = 2f;

    void Update ()
    {
        transform.localPosition = player.localPosition + orient.back * distance;
        transform.localRotation = orient.localRotation;
    }
}
