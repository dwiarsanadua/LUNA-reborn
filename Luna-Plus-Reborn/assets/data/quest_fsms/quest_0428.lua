--[[
  Quest 428: [Party][Quest]The Final Qualifying Exam
  Level Required: 83
  NPC Start: 98, NPC Complete: 98
  Prerequisites: [427]
  Rewards: EXP=2717141
]]

local fsm = require('fsm_engine')

local quest_428 = fsm:new({
    id = 428,
    name = "[Party][Quest]The Final Qualifying Exam",
    level_required = 83,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {427},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 428: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 428: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 428: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 272,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 428: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 277,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 428: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 428: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 428: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 428: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 428: Rewards given")
                self:give_rewards({
                    exp = 2717141,
                    items = {
                        { item_id = 30000308, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_428