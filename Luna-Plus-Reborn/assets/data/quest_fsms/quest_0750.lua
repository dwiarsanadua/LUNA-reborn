--[[
  Quest 750: [Quest]Real Friend
  Level Required: 5
  NPC Start: 2, NPC Complete: 7
  Rewards: EXP=93
]]

local fsm = require('fsm_engine')

local quest_750 = fsm:new({
    id = 750,
    name = "[Quest]Real Friend",
    level_required = 5,
    npc_start = 2,
    npc_complete = 7,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 750: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 750: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 750: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 750: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 750: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 750: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 7,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 750: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 750: Rewards given")
                self:give_rewards({
                    exp = 93,
                    items = {
                        { item_id = 21000420, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_750