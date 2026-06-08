--[[
  Quest 931: His Voice
  Level Required: 103
  NPC Start: 537, NPC Complete: 537
  Rewards: EXP=9884005
]]

local fsm = require('fsm_engine')

local quest_931 = fsm:new({
    id = 931,
    name = "His Voice",
    level_required = 103,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 931: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 931: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 931: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 299,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 931: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 931: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 931: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 931: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 931: Rewards given")
                self:give_rewards({
                    exp = 9884005,
                })
            end,
            transitions = {},
        },
    },
})

return quest_931