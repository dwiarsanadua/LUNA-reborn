--[[
  Quest 888: Transparent Wings
  Level Required: 96
  NPC Start: 540, NPC Complete: 540
  Rewards: EXP=10037099
]]

local fsm = require('fsm_engine')

local quest_888 = fsm:new({
    id = 888,
    name = "Transparent Wings",
    level_required = 96,
    npc_start = 540,
    npc_complete = 540,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 888: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 888: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 888: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 321,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 888: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 888: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 888: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 540,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 888: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 888: Rewards given")
                self:give_rewards({
                    exp = 10037099,
                    items = {
                        { item_id = 403, count = 40 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_888