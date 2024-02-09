using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class TutorialEnemyKeyDrop : Script
{
    public GameObject key;
    public Enemy tutorialEnemy;
    public float timeBeforeKeySpawn = 2f;
    public float currentTimeBeforeKeySpawn;
    public bool startTimer = false;

    void Start()
    {
        key.SetActive(false);
        tutorialEnemy = this.GetComponent<Enemy>();
        currentTimeBeforeKeySpawn = timeBeforeKeySpawn;
        if(tutorialEnemy == null)
        {
            Console.WriteLine("Not tutorialEnemy reference");
        }
    }

    void Update()
    {
        if(tutorialEnemy.currentHealth <= 0)
        {
            startTimer = true;
        }
        if(startTimer)
        {
            currentTimeBeforeKeySpawn -= Time.deltaTime;
            if(currentTimeBeforeKeySpawn <= 0)
            {
                if(key!= null)
                {
                    key.SetActive(true);
                    key.transform.localPosition = gameObject.transform.localPosition;
                }

                //once player collects it
                if(key == null)
                {
                    Destroy(gameObject);//to prevent crash
                    //key = null;
                    return;
                }
            }
        }

    }
}
