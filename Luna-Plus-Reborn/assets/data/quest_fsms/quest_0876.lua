--[[
  Quest 876: The Truth of the Crown 2
  Level Required: 93
  NPC Start: 93, NPC Complete: 93
  Rewards: EXP=7778329
]]

local fsm = require('fsm_engine')

local quest_876 = fsm:new({
    id = 876,
    name = "The Truth of the Crown 2",
    level_required = 93,
    npc_start = 93,
    npc_complete = 93,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 876: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 876: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 876: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 876: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 876: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 876: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 93,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 876: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 876: Rewards given")
                self:give_rewards({
                    exp = 7778329,
                    items = {
                        { item_id = 412, count = 70 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_876