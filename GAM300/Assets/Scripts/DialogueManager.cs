using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;

public class DialogueManager : Script 
{
    public static DialogueManager Instance;

    public GameObject dialogue1;
    public GameObject dialogue2;
    public GameObject dialogue3;
    public GameObject dialogue4;
    public GameObject dialogue5;
    public GameObject ai;
    public GameObject playerName;
    public GameObject dialogueBg;

    void Awake()
    {
        Instance = this;
    }

    public void Reset()
    {
        dialogue1.SetActive(false);
        dialogue2.SetActive(false);
        dialogue3.SetActive(false);
        dialogue4.SetActive(false);
        dialogue5.SetActive(false);
        ai.SetActive(false);
        playerName.SetActive(false);
        dialogueBg.SetActive(false);
    }

    public void SetState(int i)
    {
        switch (i)
        {
            case 1:
                ai.SetActive(true);
                dialogue1.SetActive(true);
                break;
            case 2:
                playerName.SetActive(true);
                dialogue2.SetActive(true);
                break;
            case 3:
                playerName.SetActive(true);
                dialogue3.SetActive(true);
                break;
            case 4:
                playerName.SetActive(true);
                dialogue4.SetActive(true);
                break;
            case 5:
                dialogue5.SetActive(true);
                break;
        }
        dialogueBg.SetActive(true);
    }
}
