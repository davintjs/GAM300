using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class DialogueManagerLevel : Script 
{
    public static DialogueManagerLevel Instance;

    public GameObject dialogueText;
    TextRenderer text;

    public AudioSource level1;
    public AudioSource level2;
    public AudioSource level3;
    public AudioSource level4;
    public AudioSource level5;
    public AudioSource level6;
    public AudioSource level7;
    public AudioSource level8;
    public AudioSource level9;

    bool startTimer;
    float Timer;
    int curr_state = 0;

    void Awake()
    {
        Instance = this;
        startTimer = false;
        text = dialogueText.GetComponent<TextRenderer>();
        curr_state = 0;
    }

    void Update()
    {
        if (startTimer)
        {
            if(Timer > 0)
            {
                Timer -= Time.deltaTime;
            }
            else
            {
                startTimer = false;
                if(curr_state == 4)
                {
                    SetState(5);
                }
                else
                {
                    dialogueText.SetActive(false);
                }
            }
        }
    }

    void setTimer (float duration)
    {
        startTimer = true;
        Timer = duration;
    }

    public void SetState(int i)
    {
        dialogueText.SetActive (true);
        switch (i)
        {
            case 1:
                text.text = "SEER: We are now at the main level of the GRID. APEX has deployed bots to secure the area, you'll have to force your way through.";
                level1.Play();
                setTimer(8f);              
                break;
            case 2:
                text.text = "SEER: The door blocking the path needs a key, there should be one on top of these pillars.";
                ObjectiveManagerLevel.Instance.SetState(1);            
                level2.Play();
                setTimer(5f);
                break;
            case 3:
                text.text = "SEER: This should open the locked door, lets proceed further in.";
                level3.Play();
                ObjectiveManagerLevel.Instance.completeObjective(1);
                ObjectiveManagerLevel.Instance.SetState(2);
                setTimer(4f);
                break;
            case 4:
                text.text = "SEER: We are approaching the platform room, the key is with the eyebot at the centre guarding this area.";
                ObjectiveManagerLevel.Instance.completeObjective(2);
                ObjectiveManagerLevel.Instance.SetState(3);
                curr_state = 4;
                level4.Play();
                setTimer(6.5f);
                break;
            case 5:
                text.text = "Defeat it and get to the other side of the room.";
                curr_state = 5;
                setTimer(3f);
                break;
            case 6:
                text.text = "SEER: This area is infested with bots! The key is in the middle of this area. Get it and move to the next area.";
                ObjectiveManagerLevel.Instance.completeObjective(3);
                ObjectiveManagerLevel.Instance.SetState(4);
                level5.Play();
                setTimer(7f);
                break;
            case 7:
                text.text = "SEER: We are in the control room now. Beware of any lurking bots stationed at this place.";
                ObjectiveManagerLevel.Instance.completeObjective(4);
                ObjectiveManagerLevel.Instance.SetState(5);
                level6.Play();
                setTimer(5f);
                break;
            case 8:
                text.text = "SEER: This is the propeller room. Be careful of those blades, they will do some serious damage.";
                ObjectiveManagerLevel.Instance.completeObjective(5);
                ObjectiveManagerLevel.Instance.SetState(6);
                level7.Play();
                setTimer(6.5f);
                break;
            case 9:
                text.text = "SEER: We are getting close, that forcefield is the final obstacle to APEX, it should go down if we destroy its generator.";
                ObjectiveManagerLevel.Instance.SetState(7);
                level8.Play();
                setTimer(9f);
                break;
            case 10:
                text.text = "SEER: The forcefiled is down! Quickly, that lift will lead us to APEX.";
                ObjectiveManagerLevel.Instance.SetState(8);
                ObjectiveManagerLevel.Instance.completeObjective(6);
                ObjectiveManagerLevel.Instance.completeObjective(7);
                level9.Play();
                setTimer(4.5f);
                break;
        }
    }
}
