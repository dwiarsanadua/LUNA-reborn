--[[
  Quest 720: [Party] Earning some Cash
  Level Required: 60
  NPC Start: 119, NPC Complete: 119
  Rewards: EXP=437964
]]

local fsm = require('fsm_engine')

local quest_720 = fsm:new({
    id = 720,
    name = "[Party] Earning some Cash",
    level_required = 60,
    npc_start = 119,
    npc_complete = 119,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 720: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 720: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 720: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 123,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 720: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 720: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 720: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 119,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 720: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 720: Rewards given")
                self:give_rewards({
                    exp = 437964,
                })
            end,
            transitions = {},
        },
    },
})

return quest_720