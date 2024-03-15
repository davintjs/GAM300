using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class ObjectiveManagerLevel : Script
{

    public static ObjectiveManagerLevel Instance;

    public GameObject level1;
    public GameObject level2;
    public GameObject level3;
    public GameObject level4;
    public GameObject level5;
    public GameObject level6;
    public GameObject level7;
    public GameObject level8;


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
                level1.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 2:
                level2.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 3:
                level3.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 4:
                level4.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 5:
                level5.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 6:
                level6.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 7:
                level7.GetComponent<SpriteRenderer>().alpha = 0.2f;
                break;
            case 8:
                level8.GetComponent<SpriteRenderer>().alpha = 0.2f;
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
                level1.SetActive(true);
                break;
            case 2:
                level2.SetActive(true);
                break;
            case 3:
                level3.SetActive(true);
                break;
            case 4:
                level4.SetActive(true);
                break;
            case 5:
                level5.SetActive(true);
                break;
            case 6:
                level6.SetActive(true);
                break;
            case 7:
                level7.SetActive(true);
                break;
            case 8:
                level8.SetActive(true);
                break;
        }
        //play UI sound
        AudioManager.instance.itemCollected.Play();
    }

}
