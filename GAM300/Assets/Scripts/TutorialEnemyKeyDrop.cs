using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class TutorialEnemyKeyDrop : Script
{
    public Enemy tutorialEnemy;
    //public float currentTimeBeforeKeySpawn;
    public bool cue = false;

    void Start()
    {
        //key.SetActive(false);
        //tutorialEnemy = this.GetComponent<Enemy>();
        //currentTimeBeforeKeySpawn = timeBeforeKeySpawn;
        if(tutorialEnemy == null)
        {
            Console.WriteLine("Not tutorialEnemy reference");
        }
    }

    IEnumerator SpawnKey()
    {
        yield return new WaitForSeconds(2f);
        gameObject.SetActive(false);
    }

    void Update()
    {
        if(tutorialEnemy.currentHealth <= 0)
        {
            //cue dialogue 
            if(!cue)
            {
                StartCoroutine(SpawnKey());
                cue = true;
                DialogueManager.Instance.SetState(6);
            }    
            
        }


        //Drop key
        //if(startTimer)
        //{
        //    currentTimeBeforeKeySpawn -= Time.deltaTime;
        //    if(currentTimeBeforeKeySpawn <= 0)
        //    {
        //        if(key!= null)
        //        {
        //            key.SetActive(true);
        //            key.transform.localPosition = gameObject.transform.localPosition;
        //        }

        //        //once player collects it
        //        if(key == null)
        //        {
        //            Destroy(gameObject);//to prevent crash
        //            //key = null;
        //            return;
        //        }
        //    }
        //}

    }
}
