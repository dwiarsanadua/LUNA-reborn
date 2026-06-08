--[[
  Quest 845: Wreck of the Elves
  Level Required: 86
  NPC Start: 505, NPC Complete: 505
  Prerequisites: [844]
  Rewards: EXP=2767851, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_845 = fsm:new({
    id = 845,
    name = "Wreck of the Elves",
    level_required = 86,
    npc_start = 505,
    npc_complete = 505,
    prerequisites = {844},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 845: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 845: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 845: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 684,
                    count = 15,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 845: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 845: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 845: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 505,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 845: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 845: Rewards given")
                self:give_rewards({
                    exp = 2767851,
                    gold = 57826,
                })
            end,
            transitions = {},
        },
    },
})

return quest_845