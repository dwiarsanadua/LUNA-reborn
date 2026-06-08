--[[
  Quest 935: Attractive Hair
  Level Required: 104
  NPC Start: 541, NPC Complete: 541
  Rewards: EXP=13661759
]]

local fsm = require('fsm_engine')

local quest_935 = fsm:new({
    id = 935,
    name = "Attractive Hair",
    level_required = 104,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 935: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 935: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 935: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 329,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 935: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 935: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 935: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 935: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 935: Rewards given")
                self:give_rewards({
                    exp = 13661759,
                })
            end,
            transitions = {},
        },
    },
})

return quest_935