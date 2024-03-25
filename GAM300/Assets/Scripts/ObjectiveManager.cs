using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class ObjectiveManager : Script
{

    public static ObjectiveManager Instance;

    public GameObject tut1;
    public GameObject tut2;
    public GameObject tut3;
    public GameObject tut4;
    public GameObject tut5;

    void Awake()
    {
        Instance = this;
    }

    void Start()
    {
        //timer = 17f;
        //d1 = false;
    }

    void Update()
    {

    }

    public void completeObjective(int i)
    {
        switch (i)
        {
            case 0:
                break;
            case 1:
                tut1.GetComponent<SpriteRenderer>().alpha = 0.2f;
                tut2.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 2:
                tut3.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 3:
                tut4.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 4:
                tut5.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
        }
        //play UI sound
        AudioManager.instance.obj_success.Play();
    }

    public void SetState(int i)
    {
        switch (i)
        {
            case 0:
                break;
            case 1:
                tut1.SetActive(true);
                tut2.SetActive(true);
                break;
            case 2:
                tut3.SetActive(true);
                break;
            case 3:
                tut4.SetActive(true);
                break;
            case 4:
                tut5.SetActive(true);
                break;
        }
        //play UI sound
        AudioManager.instance.itemCollected.Play();
    }

}
