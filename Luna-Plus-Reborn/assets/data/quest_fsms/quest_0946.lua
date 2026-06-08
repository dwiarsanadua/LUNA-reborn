--[[
  Quest 946: [Party] Richard's Story
  Level Required: 0
  NPC Start: 542, NPC Complete: 542
  Prerequisites: [945]
  Rewards: EXP=13488379, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_946 = fsm:new({
    id = 946,
    name = "[Party] Richard's Story",
    level_required = 0,
    npc_start = 542,
    npc_complete = 542,
    prerequisites = {945},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 946: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 946: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 946: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 338,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 946: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 946: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 946: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 542,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 946: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 946: Rewards given")
                self:give_rewards({
                    exp = 13488379,
                    gold = 101632,
                    items = {
                        { item_id = 21000015, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_946