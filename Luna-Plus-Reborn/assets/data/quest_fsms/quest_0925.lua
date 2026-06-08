--[[
  Quest 925: [Quest] Pirate Captian Blue Beard
  Level Required: 101
  NPC Start: 538, NPC Complete: 538
  Rewards: EXP=10442383, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_925 = fsm:new({
    id = 925,
    name = "[Quest] Pirate Captian Blue Beard",
    level_required = 101,
    npc_start = 538,
    npc_complete = 538,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 925: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 925: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 925: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 296,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 925: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 925: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 925: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 538,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 925: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 925: Rewards given")
                self:give_rewards({
                    exp = 10442383,
                    gold = 81769,
                })
            end,
            transitions = {},
        },
    },
})

return quest_925