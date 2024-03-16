using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class EnemyAudioManager : Script
{
    public static EnemyAudioManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public AudioSource RangeEnemyBullet;
    public AudioSource RangeEnemyDead;
    public AudioSource MeleeEnemyDie;
    public AudioSource MeleeEnemyAttack;
    public AudioSource MeleeEnemyInjured;
    public AudioSource EnemyAlerted1;
    public AudioSource EnemyAlerted2;
    public AudioSource EnemyAlerted3;
    public AudioSource EnemyHit;

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

    void Start()
    {

    }
}