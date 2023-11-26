using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class CombatManager : Script
{
    public static CombatManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public float damagedShakeMag = 1;
    public float damagedShakeDur = 1;
    public float hitShakeMag = 1;
    public float hitShakeDur = 1;

    void Awake()
    {
        if (instance != null)
        {
            //Debug.LogError("More than one AudioManager in the scene.");
        }
        else
        {
            instance = this;
        }
    }
}