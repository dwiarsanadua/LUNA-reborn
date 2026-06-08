--[[
  Quest 496: [Special][Quest] Fairy Queen
  Level Required: 83
  NPC Start: 98, NPC Complete: 98
  Rewards: EXP=5474970, Gold=271320
]]

local fsm = require('fsm_engine')

local quest_496 = fsm:new({
    id = 496,
    name = "[Special][Quest] Fairy Queen",
    level_required = 83,
    npc_start = 98,
    npc_complete = 98,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 496: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 496: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 496: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 373,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 496: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 496: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 496: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 98,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 496: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 496: Rewards given")
                self:give_rewards({
                    exp = 5474970,
                    gold = 271320,
                })
            end,
            transitions = {},
        },
    },
})

return quest_496