--[[
  Quest 183: Chaotic Battlefield
  Level Required: 40
  NPC Start: 34, NPC Complete: 36
  Prerequisites: [182]
  Rewards: EXP=95297, Gold=10480
]]

local fsm = require('fsm_engine')

local quest_183 = fsm:new({
    id = 183,
    name = "Chaotic Battlefield",
    level_required = 40,
    npc_start = 34,
    npc_complete = 36,
    prerequisites = {182},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 183: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 183: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 183: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 43,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 183: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 44,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 183: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 51,
                    count = 20,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 183: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 183: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 34,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 183: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 183: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 36,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 183: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 183: Rewards given")
                self:give_rewards({
                    exp = 95297,
                    gold = 10480,
                    items = {
                        { item_id = 21000097, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_183