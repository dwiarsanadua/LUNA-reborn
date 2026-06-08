--[[
  Quest 958: [Party] Sad Song 3
  Level Required: 109
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [956]
  Rewards: EXP=17049889, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_958 = fsm:new({
    id = 958,
    name = "[Party] Sad Song 3",
    level_required = 109,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {956},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 958: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 958: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 958: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 327,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 958: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 336,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 958: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 337,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 958: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 338,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 958: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 958: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 958: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 958: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 958: Rewards given")
                self:give_rewards({
                    exp = 17049889,
                    gold = 101632,
                    items = {
                        { item_id = 21000513, count = 5 },
                        { item_id = 21000514, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_958