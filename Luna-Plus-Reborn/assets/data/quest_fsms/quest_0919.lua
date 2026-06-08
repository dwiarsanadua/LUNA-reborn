--[[
  Quest 919: [Quest] Faded Pirate Necklace
  Level Required: 100
  NPC Start: 537, NPC Complete: 537
  Rewards: EXP=11363827, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_919 = fsm:new({
    id = 919,
    name = "[Quest] Faded Pirate Necklace",
    level_required = 100,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 919: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 919: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 919: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 295,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 919: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 294,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 919: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 919: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 919: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 919: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 919: Rewards given")
                self:give_rewards({
                    exp = 11363827,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_919