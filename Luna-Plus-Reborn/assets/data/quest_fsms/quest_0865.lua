--[[
  Quest 865: Brother Bear, Where?
  Level Required: 90
  NPC Start: 91, NPC Complete: 91
  Rewards: EXP=5497258
]]

local fsm = require('fsm_engine')

local quest_865 = fsm:new({
    id = 865,
    name = "Brother Bear, Where?",
    level_required = 90,
    npc_start = 91,
    npc_complete = 91,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 865: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 865: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 865: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 288,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 865: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 865: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 865: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 91,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 865: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 865: Rewards given")
                self:give_rewards({
                    exp = 5497258,
                    items = {
                        { item_id = 398, count = 1 },
                        { item_id = 398, count = 10 },
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_865