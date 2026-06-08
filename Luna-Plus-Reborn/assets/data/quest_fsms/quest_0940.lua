--[[
  Quest 940: Dust Monsters
  Level Required: 105
  NPC Start: 541, NPC Complete: 541
  Rewards: EXP=12417633, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_940 = fsm:new({
    id = 940,
    name = "Dust Monsters",
    level_required = 105,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 940: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 940: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 940: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 325,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 940: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 940: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 940: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 940: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 940: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 940: Rewards given")
                self:give_rewards({
                    exp = 12417633,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_940