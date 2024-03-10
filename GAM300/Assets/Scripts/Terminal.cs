using System;
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
    public bool interaced = false;

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
            ThirdPersonController.instance.spawnPoint = transform.localPosition + transform.forward * 2f;
            //CheckCheckpoint();

            ThirdPersonController.instance.isAtCheckpoint = true;
        }

    }

    public void CheckCheckpoint()
    {
        if(index == ThirdPersonController.instance.checkpointIndex)
        {
            //save checkpoint
            if (Input.GetKeyDown(KeyCode.E) && ThirdPersonController.instance.isAtCheckpoint == true)
            {
                Console.WriteLine("Save Checkpoint");
                //change glow of terminal
                Material mat = terminalglowMesh.material;
                mat.Set(glowMat);
                mat.color = new vec4(glowMat.color);

                //mat.metallic = metallic;

                //shift the spawn point to where the current termainal position where the player save
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
