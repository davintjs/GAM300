using BeanFactory;
using System;
using System.Collections.Generic;
using System.Diagnostics.SymbolStore;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

public class EndKey : Script
{
    public bool gameover = false;
    public GameObject mesh;

    private float timer = 0.0f;

    void Start()
    {
        
    }

    void Update()
    {
        if(gameover)
        {
            if(timer > 0.5f)
            {
                gameover = false;
                SceneManager.LoadScene("VictoryScreenMenu");
            }
            timer += Time.deltaTime;
        }
    }

    void OnTriggerEnter(PhysicsComponent rb)
    {
        //detect the player
        if (GetTag(rb) == "Player")
        {
            gameover = true;
            AudioManager.instance.keyCollected.Play();
            mesh.SetActive(false);
        }
    }
}