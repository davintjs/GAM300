﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class Terminal : Script
{
    public int index;
    public vec3 colors;
    public GameObject glowObjectReference;
    public MeshRenderer terminalglowMesh;
    public MeshRenderer glowMesh;
    public Material glowMat;

    void Start()
    {
        //get the glow color
        glowMat = glowObjectReference.GetComponent<MeshRenderer>().material;
        //glowMat = glowMesh.material;

        if (terminalglowMesh == null)
        {
            Console.WriteLine("Missing GlowMesh Reference");
        }
        if (glowMat == null)
        {
            Console.WriteLine("Missing GlowMat Reference");
        }
    }

    void Update()
    {
        CheckCheckpoint();
    }

    void OnCollisionEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("AtCheckpoint");
            ThirdPersonController.instance.checkpointIndex = index;
            //CheckCheckpoint();
        }

    }

    public void CheckCheckpoint()
    {
        if(index == ThirdPersonController.instance.checkpointIndex)
        {
            //save checkpoint
            if (Input.GetKeyDown(KeyCode.E))
            {
                Console.WriteLine("Save Checkpoint");
                //change glow of terminal
                Material mat = terminalglowMesh.material;
                mat.Set(glowMat);
                mat.color = new vec4(glowMat.color);

                //mat.metallic = metallic;

                //shift the spawn point to where the current termainal position where the player save
                ThirdPersonController.instance.spawnPoint.localPosition = new vec3(transform.position + vec3.Ones);
                ThirdPersonController.instance.spawnPoint.localRotation = new vec3(transform.rotation);
                AudioManager.instance.uiSound.Play();

            }
        }

    }

    void OnCollisionExit(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            Console.WriteLine("AwayFromCheckpoint");
            ThirdPersonController.instance.isAtCheckpoint = false;
        }
    }
}
