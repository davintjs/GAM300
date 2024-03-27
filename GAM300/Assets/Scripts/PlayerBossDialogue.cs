using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class PlayerBossDialogue : Script
{
    public ThirdPersonController player;
    private DialogueManagerBoss instance;

    bool quarterHealth = false;
    bool threequarterhealth = false;
    bool death = false;

    void Awake()
    {

    }

    void Start()
    {
        instance = DialogueManagerBoss.Instance;
    }

    void Update()
    {
        if(!threequarterhealth)
        {
            if (player.currentHealth <= (3 * player.maxHealth / 4))
            {
                instance.SetState(13); //Did i overestimate your abilities?
                threequarterhealth = true;
            }
        }
        else if (!quarterHealth)
        {
             if (player.currentHealth <= player.maxHealth /4)
            {
                instance.SetState(12); //what a joke
                quarterHealth = true;
            }
        }
        else if (!death)
        {
            if(player.currentHealth <= 0f) {
                instance.SetState(11); //HAHAHAHA
                death = true;
            }
        }

    }

   


}
