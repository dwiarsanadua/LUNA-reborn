--[[
  Quest 951: [Party]Sad Song
  Level Required: 108
  NPC Start: 541, NPC Complete: 541
  Rewards: EXP=15805326, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_951 = fsm:new({
    id = 951,
    name = "[Party]Sad Song",
    level_required = 108,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 951: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 951: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 951: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 327,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 951: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 336,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 951: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 337,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 951: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 338,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 951: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 951: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 951: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 951: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 951: Rewards given")
                self:give_rewards({
                    exp = 15805326,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_951