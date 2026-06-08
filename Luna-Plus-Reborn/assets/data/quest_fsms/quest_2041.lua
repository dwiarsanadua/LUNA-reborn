--[[
  Quest 2041: [Daily] Filling the Cracks 4
  Level Required: 91
  NPC Start: 412, NPC Complete: 412
  Rewards: EXP=2465431
]]

local fsm = require('fsm_engine')

local quest_2041 = fsm:new({
    id = 2041,
    name = "[Daily] Filling the Cracks 4",
    level_required = 91,
    npc_start = 412,
    npc_complete = 412,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 2041: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 2041: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 2041: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 289,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2041: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 2041: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 2041: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 412,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 2041: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 2041: Rewards given")
                self:give_rewards({
                    exp = 2465431,
                    items = {
                        { item_id = 30000836, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_2041