using System.Collections;
using System.Collections.Generic;
using BeanFactory;
using GlmSharp;
using System;
using System.Threading;
using System.Resources;

public class Generator : Script
{
    public GameObject forceField;
    public GameObject explosionEffect;
    public AudioSource generatorExplodeSound;
    public Transform hitEffectPos;

    public GameObject ObjectReference;
    public MeshRenderer generatorMesh;
    public MeshRenderer burntMesh;
    public Material burntMat;

    public int dialogue_state = 10;
    bool done = false;

    public float maxHealth = 5f;
    public float currentHealth;


    void Start()
    {
        currentHealth = maxHealth;

        //get the glow color
        burntMat = ObjectReference.GetComponent<MeshRenderer>().material;
        //glowMat = glowMesh.material;

        if (generatorMesh == null)
        {
            Console.WriteLine("Missing generatorwMesh Reference");
        }
        if (burntMat == null)
        {
            Console.WriteLine("Missing burntMat Reference");
        }
    }

    void Update()
    {

    }

    void TakeDamage(float amount)
    {
        ThirdPersonCamera.instance.ShakeCamera(CombatManager.instance.hitShakeMag, CombatManager.instance.hitShakeDur);
        ThirdPersonCamera.instance.SetFOV(-CombatManager.instance.hitShakeMag * 150, CombatManager.instance.hitShakeDur * 4);
        currentHealth -= amount;
        if(currentHealth <= 0)
        {
            currentHealth = 0;
            DialogueManagerLevel.Instance.SetState(dialogue_state);
            Console.WriteLine("Generatorhit");
            forceField.GetComponent<Door>().moving = true;
            generatorExplodeSound.Play();
            //change glow of terminal
            Material mat = generatorMesh.material;
            mat.Set(burntMat);
            mat.color = new vec4(burntMat.color);
            explosionEffect.SetActive(true);

            done = true;
        }
    }

    void OnTriggerEnter(PhysicsComponent other)
    {
        if (done) return;

        //check if the rigidbody belongs to a game object called PlayerWeaponCollider
        if (GetTag(other) == "PlayerAttack")
        {
            TakeDamage(1);
            CombatManager.instance.SpawnHitEffect2(hitEffectPos);
            generatorExplodeSound.Play();
            //DialogueManagerLevel.Instance.SetState(dialogue_state);
            //Console.WriteLine("Generatorhit");
            //forceField.GetComponent<Door>().moving = true;
            //generatorExplodeSound.Play();
            ////change glow of terminal
            //Material mat = generatorMesh.material;
            //mat.Set(burntMat);
            //mat.color = new vec4(burntMat.color);
            //explosionEffect.SetActive(true);
        }
    }

    public void OnTriggerExit(PhysicsComponent component)
    {
        if (done) return;
        //if (GetTag(component) == "PlayerAttack")
        //{
        //    done = true;
        //}
    }
}
